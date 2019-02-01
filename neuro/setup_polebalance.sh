#!/bin/sh
#execute this in the folder that you want to clone neuro to

git clone git@bitbucket.org:neuromorphic-utk/neuro.git neuro
cd neuro
git clone git@bitbucket.org:neuromorphic-utk/neuro_models_dummy.git models/dummy
git clone git@bitbucket.org:neuromorphic-utk/neuro_models_danna.git models/danna
git clone git@bitbucket.org:neuromorphic-utk/neuro_eo.git eo
git clone git@bitbucket.org:neuromorphic-utk/neuro_apps_polebalance.git apps/polebalance

make
(cd eo && make)
(cd models/danna && make)
(cd models/dummy && make)
sed -i -e 's/nida/danna/g' apps/polebalance/makefile
(cd apps/polebalance && make)
cd apps/polebalance && ./viz.sh nets/danna-15a.txt
