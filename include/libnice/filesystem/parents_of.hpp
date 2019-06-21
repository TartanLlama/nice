#pragma once
#include "../filesystem.hpp"

#include <utility>
#include <type_traits>
#include <range/v3/view_facade.hpp>

namespace noice
{
	struct parents_of_view
		: ranges::view_facade<parents_of_view, ranges::finite>
	{
	private:
		friend ranges::range_access;

		fs::path path_;
		struct sentinel {};
		struct cursor
		{
		private:
			parents_of_view* view_;
		public:
			using result_t = fs::path;
			cursor() = default;
			explicit cursor(parents_of_view& view)
				: view_(&view)
			{}
			fs::path const& path() const {
				return view_->path_;
			}
			result_t read() const
			{
				return view_->path_.parent_path();
			}
			void next()
			{
				view_->path_ = view_->path_.parent_path();
			}
			bool equal(sentinel) const {
				return !path().has_parent_path();
			}
		};
		cursor begin_cursor()
		{
			return cursor{ *this };
		}

		sentinel end_cursor() const
		{
			return {};
		}

		friend cursor;
	public:
		parents_of_view() = default;
		explicit parents_of_view(fs::path path)
			: path_(std::move(path))
		{}
	};

	struct parents_of_fn
	{
		parents_of_view operator()(fs::path path) const
		{
			return parents_of_view{ std::move(path) };
		}

	};

	RANGES_INLINE_VARIABLE(parents_of_fn, parents_of)
}

