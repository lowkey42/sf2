/***********************************************************\
 * Generic de-/ serializers for annotated types            *
 *     ___________ _____                                   *
 *    /  ___|  ___/ __  \                                  *
 *    \ `--.| |_  `' / /'                                  *
 *     `--. \  _|   / /                                    *
 *    /\__/ / |   ./ /___                                  *
 *    \____/\_|   \_____/                                  *
 *                                                         *
 *                                                         *
 *  Copyright (c) 2014 Florian Oetke                       *
 *                                                         *
 *  This file is part of SF2 and distributed under         *
 *  the MIT License. See LICENSE file for details.         *
\***********************************************************/

#pragma once

#include <memory>

#include "reflection_data.hpp"

namespace sf2 {

	template<typename Writer>
	struct Serializer;

	template<typename Writer>
	struct Deserializer;


	namespace details {
		using std::begin; using std::end;

		template<class Writer, class T>
		struct has_save {
			private:
				typedef char one;
				typedef long two;

				template<typename U, void (U::*)(Serializer<Writer>&)const> struct SaveMethod {};
				template <typename C> static one test(SaveMethod<C, &C::save>*);
				template <typename C> static two test(...);


			public:
				enum { value = sizeof(test<T>(0)) == sizeof(char) };
		};
		template<class Reader, class T>
		struct has_load {
			private:
				typedef char one;
				typedef long two;

				template<typename U, void (U::*)(Deserializer<Reader>&)> struct SaveMethod {};
				template <typename C> static one test(SaveMethod<C, &C::load>*);
				template <typename C> static two test(...);


			public:
				enum { value = sizeof(test<T>(0)) == sizeof(char) };
		};

		template<class T>
		struct is_range {
			private:
				typedef char one;
				typedef long two;

				template <typename C> static one test(decltype(begin(std::declval<C&>()))*, decltype(end(std::declval<C&>()))*);
				template <typename C> static two test(...);


			public:
				enum { value = sizeof(test<T>(0, 0)) == sizeof(char) };
		};

		template<class T>
		struct has_key_type {
			private:
				typedef char one;
				typedef long two;

				template <typename C> static one test(typename C::key_type*);
				template <typename C> static two test(...);


			public:
				enum { value = sizeof(test<T>(0)) == sizeof(char) };
		};

		template<class T>
		struct has_mapped_type {
			private:
				typedef char one;
				typedef long two;

				template <typename C> static one test(typename C::mapped_type*);
				template <typename C> static two test(...);


			public:
				enum { value = sizeof(test<T>(0)) == sizeof(char) };
		};

		template<class T>
		struct is_map {
			enum { value = is_range<T>::value &&
			               has_key_type<T>::value &&
			               has_mapped_type<T>::value };
		};
		template<class T>
		struct is_set {
			enum { value = is_range<T>::value &&
			               has_key_type<T>::value &&
			               !has_mapped_type<T>::value };
		};
		template<class T>
		struct is_list {
			enum { value = is_range<T>::value &&
			               !has_key_type<T>::value &&
			               !has_mapped_type<T>::value };
		};
	}

	template<typename Writer>
	struct Serializer {
		Serializer(Writer&& w) : writer(std::move(w)) {}

		template<class T>
		std::enable_if_t<is_annotated_struct<T>::value>
		  write(const T& inst) {
			writer.begin_document();

			get_struct_info<T>().for_each([&](const char* n, auto mptr) {
				write_member(n, inst.*mptr);
			});

			writer.end_current();
		}

		template<typename... Members>
		inline void write_virtual(Members&&... m) {
			writer.begin_document();

			auto i = {write_member_pair(m)...};
			(void)i;

			writer.end_current();
		}

		private:
			Writer writer;

			template<class T>
			int write_member_pair(std::pair<const char*, const T&> inst) {
				writer.write(inst.first);
				write_value(inst.second);
				return 0;
			}

			template<class T>
			void write_member(const char* name, const T& inst) {
				writer.write(name);
				write_value(inst);
			}

			template<class T>
			void write_value(const T* inst) {
				if(inst)
					write_value(*inst);
				else
					writer.write_nullptr();
			}
			template<class T>
			void write_value(const std::unique_ptr<T>& inst) {
				if(inst)
					write_value(*inst);
				else
					writer.write_nullptr();
			}
			template<class T>
			void write_value(const std::shared_ptr<T>& inst) {
				if(inst)
					write_value(*inst);
				else
					writer.write_nullptr();
			}


			// annotated struct
			template<class T>
			std::enable_if_t<is_annotated_struct<T>::value>
			  write_value(const T& inst) {
				writer.begin_obj();

				get_struct_info<T>().for_each([&](const char* n, auto mptr) {
					write_member(n, inst.*mptr);
				});

				writer.end_current();
			}

			// annotated enum
			template<class T>
			std::enable_if_t<is_annotated_enum<T>::value>
			  write_value(const T& inst) {
				writer.write(get_enum_info<T>().name_of(inst));
			}

			// map
			template<class T>
			std::enable_if_t<not is_annotated<T>::value && details::is_map<T>::value>
			  write_value(const T& inst) {

				writer.begin_obj();

				for(auto& v : inst) {
					write_value(v.first);
					write_value(v.second);
				}

				writer.end_current();
			}

			// other collection
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && (details::is_list<T>::value || details::is_set<T>::value)>
			  write_value(const T& inst) {

				writer.begin_array();

				for(auto& v : inst)
					write_value(v);

				writer.end_current();
			}

			// manual save-function
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && details::has_save<Writer,T>::value>
			  write_value(const T& inst) {
				inst.save(*this);
			}

			// other
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                && not details::is_range<T>::value
			                && not details::has_save<Writer,T>::value>
			  write_value(const T& inst) {
				writer.write(inst);
			}

			void write_value(const std::string& inst) {
				writer.write(inst);
			}
			void write_value(const char* inst) {
				writer.write(inst);
			}
	};

	template<typename Writer, typename T>
	inline void serialize(Writer&& w, const T& v) {
		Serializer<Writer>{std::move(w)}.write(v);
	}

	template<typename Writer, typename... Members>
	inline void serialize_virtual(Writer&& w, Members&&... m) {
		Serializer<Writer>{std::move(w)}.write_virtual(std::forward<Members>(m)...);
	}


	template<typename Reader>
	struct Deserializer {
		Deserializer(Reader&& r) : reader(std::move(r)) {}

		template<class T>
		std::enable_if_t<is_annotated_struct<T>::value>
		  read(T& inst) {

			while(reader.in_document()) {
				reader.read(buffer);

				get_struct_info<T>().for_each([&](const char* n, auto mptr) {
					if(buffer==n)
						read_value(inst.*mptr);
				});
			}
		}

		template<typename... Members>
		inline void read_virtual(Members&&... m) {
			while(reader.in_document()) {
				reader.read(buffer);

				auto i = {read_member_pair(buffer, m)...};
				(void)i;
			}
		}

		private:
			Reader reader;
			std::string buffer;

			template<class T>
			int read_member_pair(std::string& n, std::pair<const char*, T&> inst) {
				if(n==inst.first)
					read_value(inst.second);
				return 0;
			}

			template<class T>
			void read_value(std::unique_ptr<T>& inst) {
				if(reader.read_nullptr())
					inst = nullptr;

				else {
					inst = std::make_unique<T>();
					read_value(*inst);
				}
			}
			template<class T>
			void read_value(std::shared_ptr<T>& inst) {
				if(reader.read_nullptr())
					inst = nullptr;

				else {
					inst = std::make_shared<T>();
					read_value(*inst);
				}
			}


			// annotated struct
			template<class T>
			std::enable_if_t<is_annotated_struct<T>::value>
			  read_value(T& inst) {
				while(reader.in_obj()) {
					reader.read(buffer);

					get_struct_info<T>().for_each([&](const char* n, auto mptr) {
						if(buffer==n)
							read_value(inst.*mptr);
					});
				}
			}

			// annotated enum
			template<class T>
			std::enable_if_t<is_annotated_enum<T>::value>
			  read_value(T& inst) {
				reader.read(buffer);
				inst = get_enum_info<T>().value_of(buffer);
			}

			// map
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && details::is_map<T>::value>
			  read_value(T& inst) {
				inst.clear();

				typename T::key_type key;
				typename T::mapped_type val;
				while(reader.in_obj()) {

					read_value(key);
					read_value(val);

					inst.emplace(key, val);
				}
			}

			//set
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && details::is_set<T>::value>
			  read_value(T& inst) {
				inst.clear();

				typename T::value_type v;
				while(reader.in_array()) {
					read_value(v);

					inst.emplace(v);
				}
			}

			// other collection
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && details::is_list<T>::value>
			  read_value(T& inst) {
				inst.clear();

				typename T::value_type v;
				while(reader.in_array()) {
					read_value(v);

					inst.emplace_back(v);
				}
			}

			// manual save-function
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && details::has_load<Reader,T>::value>
			  read_value(T& inst) {
				inst.load(*this);
			}

			// other
			template<class T>
			std::enable_if_t<not is_annotated<T>::value
			                 && not details::is_range<T>::value
			                 && not details::has_load<Reader,T>::value>
			  read_value(T& inst) {
				reader.read(inst);
			}

			void read_value(std::string& inst) {
				reader.read(inst);
			}
	};

	template<typename Reader, typename T>
	inline void deserialize(Reader&& r, T& v) {
		Deserializer<Reader>{std::move(r)}.read(v);
	}

	template<typename Reader, typename... Members>
	inline void deserialize_virtual(Reader&& r, Members&&... m) {
		Deserializer<Reader>{std::move(r)}.read_virtual(std::forward<Members>(m)...);
	}

}