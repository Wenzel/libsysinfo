libsysinfo
==============

Extracts various information from `/proc` virtual filesystem

Dependencies
------------

- `Boost` (`filesystem`, `regex`)
- `Qt5` (`Widgets`, `Declarative`, `Qml`)

Setup
-----

    ./configure.sh
    make

Run
---

    ./_build/tests/test_simple
    ./_build/tests/gui_qml/test_gui_qml   

TODO
----

- bad function name for getters in ProcessInfo
- check refs
- review fields type
- check nbCore when updating cpu usage
- reorder getters
