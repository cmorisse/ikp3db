# Cloud9 Workspace Python3.6 upgrade instructions.

    wget https://www.python.org/ftp/python/3.6.1/Python-3.6.1.tgz
    tar -zxf Python-3.6.1.tgz
    cd Python-3.6.1
    sudo ./configure
    sudo make install


# Installation from sources 

## Dev mode (require write access to IKP3db repo or a fork)

This installs IKPdb in a python3.6 virtualenv (py36/src/ikpdb)

    cd .. # Leave ./Python-3.6.1 and go back to worksaace root
    sudo pip3 install virtualenv
    virtualenv py36
    source py36/bin/activate
    pip3 install --src ./ikp3db_src -e git+git@github.com:cmorisse/ikp3db.git#egg=ikp3db
     
     
## Test mode (do not require write access to repo)

This installs IKPdb in the global python3. 

    sudo pip3 install git+https://github.com/cmorisse/ikp3db.git#egg=ikp3db


# Installation from pypi

Once published, latest stable release of IKP3db will be installed using:

     sudo pip3 install ikp3db
     

