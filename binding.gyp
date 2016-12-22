{
  'includes': [ 'common.gypi' ],
  'targets': [
    {
      'target_name': '<(module_name)',
      'product_dir': '<(module_path)',
      'defines': [
        'MODULE_NAME=<(module_name)'
      ],
      'sources': [
        './src/module.cpp',
        './src/count.cpp'
      ],
      'include_dirs': [
        '<!(node -e \'require("nan")\')',
        '<!(node -e \'require("libosmium")\')',
        '<!(node -e \'require("protozero")\')'
      ],
      'ldflags': [
        '-Wl,-z,now',
      ],
      'xcode_settings': {
        'OTHER_LDFLAGS':[
          '-Wl,-bind_at_load'
        ],
        'GCC_ENABLE_CPP_RTTI': 'YES',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'MACOSX_DEPLOYMENT_TARGET':'10.8',
        'CLANG_CXX_LIBRARY': 'libc++',
        'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
        'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0'
      }
    }
  ]
}