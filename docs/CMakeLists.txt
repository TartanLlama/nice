find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR})
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND ${SPHINX_EXECUTABLE} -b html
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                     index.rst
                     api_reference/index.rst
                     git_concepts/author.rst
                     git_concepts/database.rst
                     git_concepts/index.rst
                     git_concepts/lockfile.rst
                     git_concepts/object.rst
                     git_concepts/refs.rst
                     git_concepts/sha1.rst
                     git_concepts/tree.rst
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")
 
 add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})