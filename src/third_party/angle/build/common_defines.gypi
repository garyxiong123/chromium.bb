# Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'variables':
    {
        'component%': 'shared_library',
        'angle_path%': '..',
        'windows_sdk_path%': 'C:/Program Files (x86)/Windows Kits/8.0',
    },
    'msvs_disabled_warnings': [ 4100, 4127, 4239, 4244, 4245, 4251, 4512, 4702, 4530, 4718, 4267 ],
    'msvs_system_include_dirs':
    [
        '<(windows_sdk_path)/Include/shared',
        '<(windows_sdk_path)/Include/um',
    ],
    'msvs_settings':
    {
        'VCCLCompilerTool':
        {
            'PreprocessorDefinitions':
            [
                '_CRT_SECURE_NO_DEPRECATE',
                '_SCL_SECURE_NO_WARNINGS',
                'NOMINMAX',
            ],
        },
        'VCLinkerTool':
        {
            'AdditionalDependencies':
            [
                'kernel32.lib',
                'gdi32.lib',
                'winspool.lib',
                'comdlg32.lib',
                'advapi32.lib',
                'shell32.lib',
                'ole32.lib',
                'oleaut32.lib',
                'user32.lib',
                'uuid.lib',
                'odbc32.lib',
                'odbccp32.lib',
                'delayimp.lib',
            ],
        },
    },
    'configurations':
    {
        'Debug':
        {
            'msvs_settings':
            {
                'VCLinkerTool':
                {
                    'AdditionalLibraryDirectories':
                    [
                        '<(windows_sdk_path)/Lib/win8/um/x86',
                    ],
                },
                'VCLibrarianTool':
                {
                    'AdditionalLibraryDirectories':
                    [
                        '<(windows_sdk_path)/Lib/win8/um/x86',
                    ],
                },
            },
            'defines':
            [
                '_DEBUG'
            ],
        },
        'Release':
        {
            'msvs_settings':
            {
                'VCLinkerTool':
                {
                    'AdditionalLibraryDirectories':
                    [
                        '<(windows_sdk_path)/Lib/win8/um/x86',
                    ],
                },
                'VCLibrarianTool':
                {
                    'AdditionalLibraryDirectories':
                    [
                        '<(windows_sdk_path)/Lib/win8/um/x86',
                    ],
                },
            },
            'defines':
            [
                'NDEBUG'
            ],
        },
    },
    'conditions':
    [
        ['component=="shared_library"',
        {
            'defines': [ 'COMPONENT_BUILD' ],
        }],
    ],
}
