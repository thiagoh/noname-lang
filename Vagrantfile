# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://atlas.hashicorp.com/search.
  config.vm.box = "bento/ubuntu-16.04"

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a private network, which allows host-only access to the machine
  # using a specific IP.
  # config.vm.network "private_network", ip: "192.168.33.10"

  # Create a public network, which generally matched to bridged network.
  # Bridged networks make the machine appear as another physical device on
  # your network.
  # config.vm.network "public_network"

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  # config.vm.synced_folder "../data", "/vagrant_data"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:
  #
  config.vm.provider "virtualbox" do |vb|
    # Display the VirtualBox GUI when booting the machine
    vb.gui = false
    
    vb.customize ["modifyvm", :id, "--cpuexecutioncap", "70"]

    # Customize the amount of memory on the VM:
    vb.memory = "8192"
    vb.cpus = 1
  end
  #
  # View the documentation for the provider you are using for more
  # information on available options.

  # Define a Vagrant Push strategy for pushing to Atlas. Other push strategies
  # such as FTP and Heroku are also available. See the documentation at
  # https://docs.vagrantup.com/v2/push/atlas.html for more information.
  # config.push.define "atlas" do |push|
  #   push.app = "YOUR_ATLAS_USERNAME/YOUR_APPLICATION_NAME"
  # end

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
  config.vm.provision "shell", inline: <<-SHELL

    #  install general dependencies
    apt-get -qq update
    apt-get install -y build-essential libtinfo-dev libz-dev wget subversion git htop vim
    apt-get install -y zsh 

    #  install cmake
    wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz
    tar -xzvf cmake-3.7.2.tar.gz
    cd cmake-3.7.2
    ./configure
    make -j $(nproc)
    make install

    # install oh my zsh
    sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"

    # LLVM
    # http://releases.llvm.org/download.html
    wget http://releases.llvm.org/3.9.1/llvm-3.9.1.src.tar.xz
    tar -xJvf llvm-3.9.1.src.tar.xz
    mv llvm-3.9.1.src llvm
    wget http://releases.llvm.org/3.9.1/cfe-3.9.1.src.tar.xz
    tar -xJvf cfe-3.9.1.src.tar.xz
    mv cfe-3.9.1.src clang
    wget http://releases.llvm.org/3.9.1/compiler-rt-3.9.1.src.tar.xz
    tar -xJvf compiler-rt-3.9.1.src.tar.xz
    mv compiler-rt-3.9.1.src compiler-rt
    wget http://releases.llvm.org/3.9.1/libcxx-3.9.1.src.tar.xz
    tar -xJvf libcxx-3.9.1.src.tar.xz
    mv libcxx-3.9.1.src libcxx
    wget http://releases.llvm.org/3.9.1/libcxxabi-3.9.1.src.tar.xz
    tar -xJvf libcxxabi-3.9.1.src.tar.xz
    mv libcxxabi-3.9.1.src libcxxabi
    wget http://releases.llvm.org/3.9.1/clang-tools-extra-3.9.1.src.tar.xz
    tar -xJvf clang-tools-extra-3.9.1.src.tar.xz
    mv clang-tools-extra-3.9.1.src extra

    mv clang llvm/tools/
    mv extra llvm/tools/clang/tools/
    mv compiler-rt llvm/projects/
    mv libcxx llvm/projects/
    mv libcxxabi llvm/projects/

    mkdir build
    cd build
    cmake -G "Unix Makefiles" ../llvm
    make -j $(nproc)
    make install
    
    # set Zsh to be the default shell interpreter 
    chsh -s /usr/bin/zsh
  SHELL
end
