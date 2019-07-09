PROJECT = "devilish"
PRODUCTION = "/var/www/pkgs/${PROJECT}"

node {
   stage('Test') {
      sh "make test"
   }
   stage('Compress') {
      sh "git archive --format=tar -v -o $project.tar.gz HEAD"
      sh "git archive --format=zip -v -o $project.zip HEAD"
   }
   stage('Sign') {
      sh "> sha256sums.txt"
      sh "sha256sum *.tar.gz *.zip > sha256sums.txt"
      withCredentials([string(credentialsId: 'gpgpass', variable: 'gpgpass')]) {
        sh "gpg --pinentry-mode loopback --passphrase ${gpgpass} --yes --detach-sign -a sha256sums.txt"
      }
   }
   stage('Copy') {
      sh "cp ${PROJECT}.tar.gz ${PROJECT}.zip sha256sums.txt ${PRODUCTION}"
   }
}
