#!/bin/bash

# https://support.cloudbees.com/hc/en-us/articles/203802500-Injecting-Secrets-into-Jenkins-Build-Jobs
set +x

project=devilish
production=/var/www/pkgs/$project
src=/var/www/pkgs/$project/src

cd ..
pwd
#tar -zcvf $project.tar.gz $project
git archive --format=tar -v -o $project.tar.gz $project
#zip -r $project.zip $project
git archive --format=zip -v -o $project.zip $project
sha256sum *.tar.gz *.zip > sha256sums.txt
gpg --pinentry-mode loopback --passphrase $gpgpass --batch --yes --detach-sign -a sha256sums.txt

mv $project.tar.gz $project.zip sha256sums.txt* $production
cd $src
git pull
