# example-configfile-extension

You can access properties at runtime using `sys.get_config_string()`, `sys.get_config_int()` and `sys.get_config_number()`.

The example shows how to implement config file property overrides.
The interface allows you to both override existing properties (from game.project),
but also how to return entirely new properties.
