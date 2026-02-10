#!/bin/sh

opt_clang_format="clang-format"

sd="$(dirname "$0")"
sd="$(cd "$sd" && pwd -P)"
root="$(cd "$sd/.." && pwd -P)"

print_help()
{
    cat <<EOF
Usage: $0 [options]

Options:
  -h, --help           Show this help message and exit
  -c, --clang-format   Specify clang-format tool path (default: clang-format)
EOF
}

parse_args()
{
    while [ "$#" -gt 0 ]; do
        case "$1" in
            -h|--help)
                print_help
                exit 0
                ;;
            -c|--clang-format)
                shift
                opt_clang_format="$1"
                ;;
            *)
                echo "Unknown option: $1" >&2
                print_help
                exit 1
                ;;
        esac
        shift
    done
}

check_tools()
{
    if ! command -v "$opt_clang_format" >/dev/null 2>&1; then
        echo "Error: clang-format tool not found at '$opt_clang_format'" >&2
        exit 1
    fi
    if ! command -v dos2unix >/dev/null 2>&1; then
        echo "Error: dos2unix tool not found" >&2
        exit 1
    fi
}

main()
{
    parse_args "$@"
    check_tools

    cd "$root"
    cxx_sources="$(git ls-files | grep -E '\.cpp$|\.h$')"
    echo "$cxx_sources" | xargs "$opt_clang_format" -i
    echo "$cxx_sources" | xargs dos2unix -q --add-bom

    script_sources="$(git ls-files | grep -E 'CMakeLists\.txt$|\.cmake$|\.sh$')"
    echo "$script_sources" | xargs dos2unix -q

    # Add UTF-8 BOM and convert to LF for doc files (README and LICENSE)
    doc_files="$(git ls-files | grep -E 'README\.md$|LICENSE(\.md)?$')"
    echo "$doc_files" | xargs dos2unix -q --add-bom
}

main "$@"
