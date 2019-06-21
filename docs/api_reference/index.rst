API Reference
=============


Git Components
--------------

.. cpp:class:: index
.. cpp:class:: workspace
.. cpp:class:: refs
.. cpp:type:: sha1_digest = std::array<std::byte, 20>

  A :doc:`/git_concepts/sha1` expressed as an array of bytes

Database
^^^^^^^^

.. cpp:class:: author

    **Special Members**

    .. cpp:function:: author(std::string name, std::string email, std::time_t time)
    
    **Public Functions**

    .. cpp:function:: friend std::string to_string(author const&)

      Formats the author to a string in the format:
      
      `<name> \\<<email>\\> <unix timestamp> <UTC offset>`

.. cpp:class:: blob

    **Special Members**

    .. cpp:function:: blob(std::string contents)

    **Public Functions**

    .. cpp:function:: friend std::string to_string(blob const&)

      Formats the blob to a string in the format:
      
      `<contents>`

    .. cpp:function:: sha1_digest const& oid() const

      Get the object ID of the blob

.. cpp:class:: commit

   Represents a Git :doc:`Commit </git_concepts/commit>`

   **Special Members**

   .. cpp:function:: commit(tl::optional<sha1_digest> const& parent, sha1_digest const& tree_oid, author auth, std::string message)

   **Public Functions**

   .. cpp:function:: std::string_view type() const

      The :ref:`database object type <database-object-type>`, which is "commit"
.. cpp:class:: database

   Represents a Git :doc:`Database </git_concepts/database>`
   
.. cpp:class:: tree

   Represents a Git :doc:`Tree </git_concepts/tree>`

.. cpp:class:: lockfile

   Represents a Git :doc:`Lockfile </git_concepts/lockfile>`

   **Special Members**

   .. cpp:function:: lockfile(fs::path path, bool binary_mode = false)
   .. cpp:function:: ~lockfile()
    
      Commit the changes made to the locked file

   **Public Functions**

   .. cpp:function:: void abort()

      Unlock the file and discard any changes written

   .. cpp:function:: void write(std::string_view)
                     template<std::size_t N> void write(std::array<std::byte,N> const&)

      Write the given data to the lockfile


Utilities
---------

Filesystem
^^^^^^^^^^
Format
^^^^^^
Functional
^^^^^^^^^^
Hash
^^^^
Binary
^^^^^^