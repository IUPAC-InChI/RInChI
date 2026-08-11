# RInChI regression tests

A Docker setup for comparing RInChI outputs against expected values from any directory that contains paired .rxn and .rxn.rinchi_strings.txt files.

## Build

Run these commands from the repository root:

```bash
docker build -f src/test/Dockerfile -t rinchi-regression-test .
```

## Run

The container requires one positional argument: the directory that contains the .rxn files and their matching .rxn.rinchi_strings.txt expectation files.

```bash
docker run --rm -v "<path/to/your/files>:/data" rinchi-regression-test /data
```

For example:

```bash
docker run --rm -v "$PWD/src/test/Cambridge_rxnfiles:/data" rinchi-regression-test /data
```

## Output

Each processed .rxn file is logged. Failed comparisons include the expected and computed values for the RInChI string and RAuxInfo.
