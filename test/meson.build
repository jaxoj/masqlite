cmocka = dependency('cmocka')

include_dir = include_directories('../include')

storage_engine_sources = ['test_storage_engine.c', '../src/storage_engine.c']
storage_engine_test = executable('test_storage_engine', storage_engine_sources, dependencies: cmocka, include_directories: include_dir)

test('storage engine unit test', storage_engine_test)