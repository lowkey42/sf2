/***********************************************************\
 * structures that describe the collected enum/struct info *
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

#include <string>
#include <map>
#include <unordered_map>
#include <array>

namespace sf2 {

	template<typename T>
	class Enum_info {
		public:
			using Value_type = std::pair<T, const char*>;

		public:
			Enum_info(const char* name, std::initializer_list<Value_type> v)
			    : _name(name) {
				for(auto& e : v) {
					_names.emplace(e.first, e.second);
					_values.emplace(e.second, e.first);
				}
			}

			auto name()const noexcept {return _name;}

			auto value_of(const std::string& name)const noexcept -> T {
				auto i = _values.find(name);
				assert(i!=_values.end());
				return i->second;
			}

			auto name_of (T value)const noexcept -> std::string {
				auto i = _names.find(value);
				assert(i!=_names.end());
				return i->second;
			}

		private:
			const char* _name;
			std::map<T, std::string> _names;
			std::unordered_map<std::string, T> _values;
	};


	template<typename ST, typename MT>
	using Member_ptr = MT ST::*;

	template<typename T, typename MemberT>
	using Member_data = std::tuple<MemberT T::*, const char*>;

	template<typename T, typename... MemberT>
	class Struct_info {
		public:
			static constexpr std::size_t member_count = sizeof...(MemberT);

			constexpr Struct_info(const char* name, Member_data<T,MemberT>... members) : _name(name), _member_names{{std::get<1>(members)...}}, _members(std::get<0>(members)...) {
			}

			constexpr auto name()const {return _name;}
			constexpr auto& members()const {return _member_names;}
			constexpr auto& member_ptrs()const {return _members;}
			constexpr auto size()const {return member_count;}


			template<std::size_t I = 0, typename FuncT>
			inline typename std::enable_if<I == member_count, void>::type
			for_each( FuncT)const {}

			template<std::size_t I = 0, typename FuncT>
			inline typename std::enable_if<I < member_count, void>::type
			for_each(FuncT f)const {
				f(_member_names[I], std::get<I>(_members));
				for_each<I + 1, FuncT>(f);
			}

		private:
			const char* _name;
			std::array<const char*, member_count> _member_names;
			std::tuple<Member_ptr<T,MemberT>...> _members;
	};


	template<typename T>
	auto get_enum_info() -> decltype(sf2_enum_info_factory((T*)nullptr)) {
		return sf2_enum_info_factory((T*)nullptr);
	}

	template<typename T>
	auto get_struct_info() -> decltype(sf2_struct_info_factory((T*)nullptr)) {
		return sf2_struct_info_factory((T*)nullptr);
	}


	template<class T>
	struct is_annotated_struct {
		private:
			typedef char one;
			typedef long two;

			template <typename C> static one test( std::remove_reference_t<decltype(sf2_struct_info_factory((C*)nullptr))>* ) ;
			template <typename C> static two test(...);


		public:
			enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

	template<class T>
	struct is_annotated_enum {
		private:
			typedef char one;
			typedef long two;

			template <typename C> static one test( std::remove_reference_t<decltype(sf2_enum_info_factory((C*)nullptr))>* ) ;
			template <typename C> static two test(...);


		public:
			enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

	template<class T>
	struct is_annotated {
		enum { value = is_annotated_struct<T>::value ||
		               is_annotated_enum<T>::value };
	};

}
