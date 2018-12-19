#!/bin/bash
set +x

project=devilish
production=/var/www/pkgs/$project
src=/var/www/pkgs/$project/src
demo=/var/www/demo/programs/ 
obj=devilish.out

#tar -zcvf $project.tar.gz $project
git archive --format=tar -v -o $project.tar.gz HEAD
#zip -r $project.zip $project
git archive --format=zip -v -o $project.zip HEAD
sha256sum *.tar.gz *.zip > sha256sums.txt
gpg --pinentry-mode loopback --passphrase $gpgpass --batch --yes --detach-sign -a sha256sums.txt

mv $project.tar.gz $project.zip sha256sums.txt* $production
cd $src
git pull
make
mv $obj $demo
