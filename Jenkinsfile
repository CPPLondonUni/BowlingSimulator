pipeline {
	agent any
	stages {
		stage('Build') {
			steps {
				dir('build') {
					sh 'cmake ..'
					sh 'cmake --build  .'
				}
			}
		}
	}
}
