cmocka = dependency('cmocka')

include_dir = include_directories('../include')

storage_engine_sources = ['test_storage_engine.c', '../src/storage_engine.c', ]
storage_engine_test = executable(
    'test_storage_engine',
    storage_engine_sources,
    dependencies : cmocka,
    include_directories : include_dir
)

btree_sources = ['test_btree.c', '../src/btree.c']
btree_test = executable(
    'test_btree',
    btree_sources,
    dependencies : cmocka,
    include_directories : include_dir,
)


vm_sources = ['test_vm.c', '../src/vm.c', '../src/btree.c']
vm_test = executable(
    'test_virtual_machine',
    vm_sources,
    dependencies : cmocka,
    include_directories : include_dir
)

sql_lexer_sources = ['test_sql_lexer.c', '../src/sql/lexer.c']
sql_lexer_test = executable(
    'test_sql_lexer',
    sql_lexer_sources,
    dependencies : cmocka,
    include_directories : include_dir
)

sql_parser_sources = ['test_sql_parser.c', '../src/sql/parser.c', '../src/sql/lexer.c']
sql_parser_test = executable(
    'test_sql_parser',
    sql_parser_sources,
    dependencies : cmocka,
    include_directories : include_dir
)

test('storage engine unit tests', storage_engine_test)
test('btree unit tests', btree_test)
test('virtual machine unit tests', vm_test)
test('sql lexer unit tests', sql_lexer_test)
test('sql parser unit tests', sql_parser_test)
