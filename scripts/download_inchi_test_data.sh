wget https://www.inchi-trust.org/wp/download/106/INCHI-1-TEST.zip

## If download fails with an expired certificate, use this command instead.
## wget https://www.inchi-trust.org/wp/download/106/INCHI-1-TEST.zip --no-check-certificate

unzip INCHI-1-TEST.zip

cd INCHI-1-TEST/test
unzip test-datasets.zip
unzip test-results.zip
cd ../..
