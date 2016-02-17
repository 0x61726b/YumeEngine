################################################################################
#Yume Engine MIT License (MIT)

# Copyright (c) 2015 arkenthera
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# File : CmakeLists.txt
# Date : 8.27.2015
# Comments :
################################################################################


#Log4cplus is a very powerful logging libarary
ADD_SUBDIRECTORY(log4cplus)

#Boost
add_boost_library(system)
add_boost_library(filesystem)
add_boost_library(thread)
add_boost_library(date_time)

set_target_properties(boost_system PROPERTIES FOLDER "3rdParty" )
set_target_properties(boost_filesystem PROPERTIES FOLDER "3rdParty" )
set_target_properties(boost_thread PROPERTIES FOLDER "3rdParty" )
set_target_properties(boost_date_time PROPERTIES FOLDER "3rdParty" )

if(OS_LINUX)
  target_compile_options(boost_system PUBLIC "-fPIC")
  target_compile_options(boost_filesystem PUBLIC "-fPIC")
  target_compile_options(boost_thread PUBLIC "-fPIC")
  target_compile_options(boost_date_time PUBLIC "-fPIC")
endif()
