#!/bin/bash

PROJECT_DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $PROJECT_DIR
echo "Project path: $PROJECT_DIR"

# clear data/www
rm -rf $PROJECT_DIR/data/www

# create destination directories
mkdir -p $PROJECT_DIR/data/www/js
mkdir -p $PROJECT_DIR/data/www/css

# create js/css for depenencies
cd $PROJECT_DIR/www/js
cat jquery.min.js bootstrap.min.js knockout.js select.min.js bootstrap4-toggle.min.js > deps.min.js
cd $PROJECT_DIR/www/css
cat bootstrap.min.css select.min.css bootstrap4-toggle.min.css style.css > layout.css

# zip webinterface stuff to data/www
cd $PROJECT_DIR
cat www/js/deps.min.js | gzip -9 > data/www/js/deps.min.js.gz
cat www/js/script.js | gzip -9 > data/www/js/script.js.gz
cat www/css/layout.css | gzip -9 > data/www/css/layout.css.gz
cat www/index.htm | gzip > data/www/index.htm.gz

# brotli www/js/deps.min.js -o data/www/js/deps.min.js.br
# brotli www/js/script.js -o data/www/js/script.js.br
# brotli www/css/layout.css -o data/www/css/layout.css.br
# brotli www/index.htm -o data/www/index.htm.br

echo "done!"
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