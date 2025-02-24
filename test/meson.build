cmocka = dependency('cmocka')

include_dir = include_directories('../include')

storage_engine_sources = ['test_storage_engine.c', '../src/storage_engine.c']
storage_engine_test = executable('test_storage_engine', storage_engine_sources, dependencies: cmocka, include_directories: include_dir)

btree_sources = ['test_btree.c', '../src/btree.c']
btree_test = executable('test_btree', btree_sources, dependencies: cmocka, include_directories: include_dir)


vm_sources = ['test_vm.c', '../src/vm.c']
vm_test = executable('test_virtual_machine', vm_sources, dependencies: cmocka, include_directories: include_dir)


test('storage engine unit tests', storage_engine_test)
test('btree unit tests', btree_test)
test('virtual machine unit tests', vm_test)