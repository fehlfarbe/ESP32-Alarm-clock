#!/bin/bash

cd "$( dirname "${BASH_SOURCE[0]}" )"

rm -rf data/www

# create directories
mkdir -p data/www/js
mkdir -p data/www/css

# create js/css for depenencies
cd www/js
cat jquery.min.js bootstrap.min.js knockout.js select.min.js > deps.min.js
cd ../css
cat bootstrap.min.css select.min.css style.css > layout.css

# change back to project dir
cd ../..
ls
cat www/js/deps.min.js | gzip -9 > data/www/js/deps.min.js.gz
cat www/js/script.js | gzip -9 > data/www/js/script.js.gz
cat www/css/layout.css | gzip -9 > data/www/css/layout.css.gz


# copy and zip JS/CSS files
# for d in js css
# do
#     for f in `ls www/${d}/*.${d}`
#     do
#         echo "Compress ${f} to data/${f}.gz"
#         cat ${f} | gzip -9 > data/${f}.gz
#     done
# done

# copy html
cat www/index.htm | gzip > data/www/index.htm.gz