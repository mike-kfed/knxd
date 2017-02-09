###############################################################################
#
# Package header
#
Summary:      A KNX daemon and tools
Name:         knxd
Version:      0.10.17
Release:      0%{?dist}
Group:        Applications/Interpreters
Source:       %{name}-%{version}.tar.gz
URL:          https://github.com/knxd/knxd/
Distribution: CentOS
Vendor:       smurf
License:      GPL
Packager:     Michael Kefeder <m.kefeder@gmail.com>

%if 0%{?rhel} >= 7
BuildRequires: systemd
BuildRequires: systemd-devel
%endif
BuildRequires: libpthsem-devel

###############################################################################
%description
A KNX daemon and tools supporting it. 

###############################################################################
%prep
%autosetup -n %{name}-%{version}

###############################################################################
%build
%configure
# make_build macro does not exist on rhel6
%if 0%{?make_build:1}
%make_build
%else
make
%endif

###############################################################################
%install
%make_install

###############################################################################
%files
%defattr(-,root,root,-)

# /usr/bin
%{_bindir}/bcuaddrtab
%{_bindir}/bcuread
%{_bindir}/eibnetdescribe
%{_bindir}/eibnetsearch
%{_bindir}/knxd
%{_bindir}/knxtool

# /usr/include
%{_includedir}/eibclient.h
%{_includedir}/eibloadresult.h
%{_includedir}/eibtypes.h

%{_libdir}/libeibclient.a
%{_libdir}/libeibclient.la
%{_libdir}/libeibclient.so
%{_libdir}/libeibclient.so.0
%{_libdir}/libeibclient.so.0.0.0

# when _unitdir macro is defined then systemd-devel is present
# this means we must mention the systemd files
%if 0%{?_unitdir:1}
%config(noreplace) %{_sysconfdir}/knxd.conf
%{_unitdir}/knxd.service
%{_unitdir}/knxd.socket
%{_sysusersdir}/knxd.conf
# TODO: copy init.d scripts and config for non-systemd OS
#%else
%endif

%{_libdir}/knxd/busmonitor1
%{_libdir}/knxd/busmonitor2
%{_libdir}/knxd/busmonitor3
%{_libdir}/knxd/eibread-cgi
%{_libdir}/knxd/eibwrite-cgi
%{_libdir}/knxd/groupcacheclear
%{_libdir}/knxd/groupcachedisable
%{_libdir}/knxd/groupcacheenable
%{_libdir}/knxd/groupcachelastupdates
%{_libdir}/knxd/groupcacheread
%{_libdir}/knxd/groupcachereadsync
%{_libdir}/knxd/groupcacheremove
%{_libdir}/knxd/grouplisten
%{_libdir}/knxd/groupread
%{_libdir}/knxd/groupreadresponse
%{_libdir}/knxd/groupresponse
%{_libdir}/knxd/groupsocketlisten
%{_libdir}/knxd/groupsocketread
%{_libdir}/knxd/groupsocketswrite
%{_libdir}/knxd/groupsocketwrite
%{_libdir}/knxd/groupsresponse
%{_libdir}/knxd/groupswrite
%{_libdir}/knxd/groupwrite
%{_libdir}/knxd/madcread
%{_libdir}/knxd/maskver
%{_libdir}/knxd/mmaskver
%{_libdir}/knxd/mpeitype
%{_libdir}/knxd/mprogmodeoff
%{_libdir}/knxd/mprogmodeon
%{_libdir}/knxd/mprogmodestatus
%{_libdir}/knxd/mprogmodetoggle
%{_libdir}/knxd/mpropdesc
%{_libdir}/knxd/mpropread
%{_libdir}/knxd/mpropscan
%{_libdir}/knxd/mpropscanpoll
%{_libdir}/knxd/mpropwrite
%{_libdir}/knxd/mread
%{_libdir}/knxd/mrestart
%{_libdir}/knxd/msetkey
%{_libdir}/knxd/mwrite
%{_libdir}/knxd/mwriteplain
%{_libdir}/knxd/progmodeoff
%{_libdir}/knxd/progmodeon
%{_libdir}/knxd/progmodestatus
%{_libdir}/knxd/progmodetoggle
%{_libdir}/knxd/readindividual
%{_libdir}/knxd/vbusmonitor1
%{_libdir}/knxd/vbusmonitor1poll
%{_libdir}/knxd/vbusmonitor1time
%{_libdir}/knxd/vbusmonitor2
%{_libdir}/knxd/vbusmonitor3
%{_libdir}/knxd/writeaddress
%{_libdir}/knxd/xpropread
%{_libdir}/knxd/xpropwrite

# /usr/share/knxd/
%{_datarootdir}/knxd/EIBConnection.cs
%{_datarootdir}/knxd/EIBConnection.lua
%{_datarootdir}/knxd/EIBConnection.pm
%{_datarootdir}/knxd/EIBConnection.py
%{_datarootdir}/knxd/EIBConnection.rb
%{_datarootdir}/knxd/EIBD.pas
%{_datarootdir}/knxd/eibclient.php
%{_datarootdir}/knxd/examples/busmonitor1.c
%{_datarootdir}/knxd/examples/busmonitor2.c
%{_datarootdir}/knxd/examples/busmonitor3.c
%{_datarootdir}/knxd/examples/eibread-cgi.c
%{_datarootdir}/knxd/examples/eibwrite-cgi.c
%{_datarootdir}/knxd/examples/groupcacheclear.c
%{_datarootdir}/knxd/examples/groupcachedisable.c
%{_datarootdir}/knxd/examples/groupcacheenable.c
%{_datarootdir}/knxd/examples/groupcachelastupdates.c
%{_datarootdir}/knxd/examples/groupcacheread.c
%{_datarootdir}/knxd/examples/groupcachereadsync.c
%{_datarootdir}/knxd/examples/groupcacheremove.c
%{_datarootdir}/knxd/examples/grouplisten.c
%{_datarootdir}/knxd/examples/groupread.c
%{_datarootdir}/knxd/examples/groupreadresponse.c
%{_datarootdir}/knxd/examples/groupresponse.c
%{_datarootdir}/knxd/examples/groupsocketlisten.c
%{_datarootdir}/knxd/examples/groupsocketread.c
%{_datarootdir}/knxd/examples/groupsocketswrite.c
%{_datarootdir}/knxd/examples/groupsocketwrite.c
%{_datarootdir}/knxd/examples/groupsresponse.c
%{_datarootdir}/knxd/examples/groupswrite.c
%{_datarootdir}/knxd/examples/groupwrite.c
%{_datarootdir}/knxd/examples/madcread.c
%{_datarootdir}/knxd/examples/maskver.c
%{_datarootdir}/knxd/examples/mmaskver.c
%{_datarootdir}/knxd/examples/mpeitype.c
%{_datarootdir}/knxd/examples/mprogmodeoff.c
%{_datarootdir}/knxd/examples/mprogmodeon.c
%{_datarootdir}/knxd/examples/mprogmodestatus.c
%{_datarootdir}/knxd/examples/mprogmodetoggle.c
%{_datarootdir}/knxd/examples/mpropdesc.c
%{_datarootdir}/knxd/examples/mpropread.c
%{_datarootdir}/knxd/examples/mpropscan.c
%{_datarootdir}/knxd/examples/mpropscanpoll.c
%{_datarootdir}/knxd/examples/mpropwrite.c
%{_datarootdir}/knxd/examples/mread.c
%{_datarootdir}/knxd/examples/mrestart.c
%{_datarootdir}/knxd/examples/msetkey.c
%{_datarootdir}/knxd/examples/mwrite.c
%{_datarootdir}/knxd/examples/mwriteplain.c
%{_datarootdir}/knxd/examples/progmodeoff.c
%{_datarootdir}/knxd/examples/progmodeon.c
%{_datarootdir}/knxd/examples/progmodestatus.c
%{_datarootdir}/knxd/examples/progmodetoggle.c
%{_datarootdir}/knxd/examples/readindividual.c
%{_datarootdir}/knxd/examples/vbusmonitor1.c
%{_datarootdir}/knxd/examples/vbusmonitor1poll.c
%{_datarootdir}/knxd/examples/vbusmonitor1time.c
%{_datarootdir}/knxd/examples/vbusmonitor2.c
%{_datarootdir}/knxd/examples/vbusmonitor3.c
%{_datarootdir}/knxd/examples/writeaddress.c
%{_datarootdir}/knxd/examples/xpropread.c
%{_datarootdir}/knxd/examples/xpropwrite.c

%exclude
%{_datarootdir}/knxd/EIBConnection.pyc
%{_datarootdir}/knxd/EIBConnection.pyo

###############################################################################
# preinstall
%pre
/usr/bin/getent group knxd > /dev/null || /usr/sbin/groupadd -r knxd
/usr/bin/getent passwd knxd > /dev/null || /usr/sbin/useradd -r -s /sbin/nologin -g knxd knxd

###############################################################################
# postinstall
%post

###############################################################################
# pre-remove script
%preun


###############################################################################
# post-remove script
%postun
if [ "$1" = "1" ]; then
    # this is an upgrade do nothing
    echo -n
elif [ "$1" = "0" ]; then
    # this is an uninstall remove user
    userdel --force knxd 2> /dev/null; true
fi

###############################################################################
# verify
%verifyscript

%changelog
* Tue Jan 10 2017 Michael Kefeder <m.kefeder@gmail.com> 0.10.17-0
- Initial crossplatform spec file created and tested on CentOS 6/7
- remove users only on uninstall in %postun hook
- use normal specfile layout
