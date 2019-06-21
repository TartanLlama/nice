#pragma once

#include <string>
#include "../hash.hpp"

namespace noice::database {
/**
 * Represents a blob (file)
 * Stores contents of the blob
 */
class blob {
    public:
        blob(std::string contents) 
          : contents_(std::move(contents))
        {}

        std::string_view type() const {
            return "blob";
        }

        /**
         * Blobs format to:
         *   <contents>
         */
        friend std::string to_string(blob const& b) {
            return b.contents_;
        }

        sha1_digest const& oid() const {
            return oid_;
        }
    private:
        std::string contents_;
        sha1_digest oid_;
        friend class database;
    };
}