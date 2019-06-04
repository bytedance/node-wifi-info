{
    "targets":[{
        "target_name": "node-wifi-info",
        "include_dirs": [
            "src",
            "<!(node -e \"require('nan')\")"
        ],
        "sources": [
            "src/index.cc"
        ]
    }]
}