solutions = [
      { "name"        : "30.0.1599.101",
        "url"         : "https://src.chromium.org/chrome/releases/30.0.1599.101",
        "deps_file"   : "DEPS",
        "managed"     : True,
        "custom_deps" : {
            "build": None,
            "build/scripts/command_wrapper/bin": None,
            "build/scripts/gsd_generate_index": None,
            "build/scripts/private/data/reliability": None,
            "build/third_party/gsutil": None,
            "build/third_party/lighttpd": None,
            "build/third_party/swarm_client": None,
            "commit-queue": None,
            "depot_tools": None,
            "chromeos": None,
            'build/third_party/cbuildbot_chromite': None,
            'build/third_party/xvfb': None,
            'build/xvfb': None,

            "src/third_party/WebKit/LayoutTests": None,
            "src/content/test/data/layout_tests/LayoutTests": None,
            "src/chrome/tools/test/reference_build/chrome_win": None,
            "src/chrome_frame/tools/test/reference_build/chrome_win": None,
            "src/chrome/tools/test/reference_build/chrome_linux": None,
            "src/chrome/tools/test/reference_build/chrome_mac": None,
            "src/third_party/hunspell_dictionaries": None,
            "src/native_client": None,
        },
        "safesync_url": "",
    },
]
