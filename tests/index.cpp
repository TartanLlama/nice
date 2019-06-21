#include <catch.hpp>
#include <libnice/filesystem.hpp>
#include <libnice/hash.hpp>
#include <libnice/index.hpp>

TEST_CASE("index") {
	auto tmp_path = fs::path("tmp") / "index";
  fs::create_directories(tmp_path);
  auto index_path = tmp_path / "index";
	fs::remove(index_path);
	auto index = noice::index(index_path);

	auto stat = noice::stat(__FILE__);
	auto oid = noice::sha1_digest{};

	SECTION("add a single file") {
		index.add("alice.txt", oid, stat);
		REQUIRE(index.entries().size() == 1);
		REQUIRE(index.entries().front().path() == "alice.txt");
		fs::remove(index_path);
	}

	SECTION("replace a file with a directory") {
		index.add("alice.txt", oid, stat);
		index.add("bob.txt", oid, stat);

		index.add("alice.txt/nested.txt", oid, stat);

		REQUIRE(index.entries().size() == 2);
		REQUIRE(index.entries()[0].path() == "alice.txt/nested.txt");
		REQUIRE(index.entries()[1].path() == "bob.txt");
		fs::remove(index_path);
	}

	SECTION("replace a directory with a file") {
		index.add("alice.txt", oid, stat);
		index.add("nested/bob.txt", oid, stat);
		
		index.add("nested", oid, stat);

		REQUIRE(index.entries().size() == 2);
		REQUIRE(index.entries()[0].path() == "alice.txt");
		REQUIRE(index.entries()[1].path() == "nested");
		fs::remove(index_path);
	}

	SECTION("recursively replace a directory with a file") {
		index.add("alice.txt", oid, stat);
		index.add("nested/bob.txt", oid, stat);
		index.add("nested/inner/claire.txt", oid, stat);

		index.add("nested", oid, stat);

		REQUIRE(index.entries().size() == 2);
		REQUIRE(index.entries()[0].path() == "alice.txt");
		REQUIRE(index.entries()[1].path() == "nested");
		fs::remove(index_path);

	}
}