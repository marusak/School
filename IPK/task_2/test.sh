#!/usr/bin/env bash
# WELCOME
echo -e "\n\n     Hello, this is demonstration! Sit back and enjoy!"

#MAKE
printf "\nFirstly, lets compile it:"
make >/dev/null
if [ $? -ne 0 ]; then
  echo -e "\n\n         Something went horribly wrong, abort mission!\n"
  exit 1
else
  echo -e " OK\n"
fi

#CREATE DIR
printf "Now, lets create folder TEST: "
mkdir -p TEST
if [ $? -ne 0 ]; then
  echo -e "\n\n         Something went horribly wrong, abort mission!\n"
  exit 1
else
  echo -e "OK\n"
fi

# COPY client into TEST
printf "Now, client will be copied into TEST: "
cp client TEST
if [ $? -ne 0 ]; then
  echo -e "\n\n         Something went horribly wrong, abort mission!\n"
  exit 1
else
  echo -e "OK\n"
fi

# DUPLICIT of client
printf "Create copy of client, name it client1: "
cp client TEST/client1
if [ $? -ne 0 ]; then
  echo -e "\n\n         Something went horribly wrong, abort mission!\n"
  exit 1
else
  echo -e "OK\n"
fi

#create TESTFILE
printf "Finally we create one test file: "
echo "This is testFile and we will copy it a bit later. Stay tuned!" >TEST/testfile.txt
echo -e "OK\n"

#Create server
echo -e "Now we fire up the server. Maximum 5 tries will be performed. (In case some ports are used...)\n"
b=1039
for i in {1..5}
do
  h=$(( $i * $b + $i))
  pid="./server -p "$h""
  $pid >/dev/null &
  pid=$!
  ps -p "$pid" >/dev/null
  if [ $? -eq 0 ]; then
    break
  fi
  echo "                Host $h unavailable\n"
done
echo -e "OK - server is on port "$h"\n"
# TODO if non running - error

#DOWNLOAD normal file
echo -e "To demonstrate my skill, now I perform download of regular file.\n"
cd TEST
printf "Download of client.cc : "
./client -h localhost -p "$h" -d client.cc
if [ $? -ne 0 ]; then
  echo -e "              \nCould not download"
  exit 1
else
  diff ../client.cc client.cc
    if [ $? -ne 0 ]; then
      echo -e "              \nFiles are not same, I cant even..."
      exit 1
    else
      echo -e "OK + checked with diff\n"
    fi
fi

#DOWNLOAD binary file
echo -e "I am getting cocky, so I try to download binary file.\n"
printf "Download of server : "
./client -h localhost -p "$h" -d server
if [ $? -ne 0 ]; then
  echo -e "              \nCould not download"
  exit 1
else
  diff ../server server
    if [ $? -ne 0 ]; then
      echo -e "              \nFiles are not same, I cant even..."
      exit 1
    else
      echo -e "OK + checked with diff\n"
    fi
fi


#UPLOAD regular file
echo -e "Download was easy, lets try upload. We will start with regular file\n"
printf "Upload of test file : "
./client -h localhost -p "$h" -u testfile.txt
if [ $? -ne 0 ]; then
  echo -e "              \nCould not upload"
  exit 1
else
  diff ../testfile.txt testfile.txt
    if [ $? -ne 0 ]; then
      echo -e "              \nFiles are not same, I cant even..."
      exit 1
    else
      echo -e "OK + checked with diff\n"
    fi
fi

#UPLOAD binary file
echo -e "Finally upload of bianry file\n"
printf "Upload of client1 : "
./client -h localhost -p "$h" -u client1
if [ $? -ne 0 ]; then
  echo -e "              \nCould not upload"
  exit 1
else
  diff ../client1 client1
    if [ $? -ne 0 ]; then
      echo -e "              \nFiles are not same, I cant even..."
      exit 1
    else
      echo -e "OK + checked with diff\n"
    fi
fi

echo -e "Now some fail tests\n"
echo -e "First, missing argument:\n"
./client -h localhost -u client
echo -e "    Return code: "$?"\n"


echo -e "Test of unknown file, upload:\n"
./client -h localhost -p "$h" -u unknown
echo -e "    Return code: "$?"\n"

echo -e "Test of unknown file, download:\n"
./client -h localhost -p "$h" -d unknownier
echo -e "    Return code: "$?"\n"


echo -e "\n\nThis is the end, my only friend, the end!\n"
printf "Lets kill server: "
kill $pid
wait $pid 2>/dev/null
echo -e "OK\n"
printf "Lets clean TEST folder, and compiled files: "
cd ..
rm -rf TEST >/dev/null
make clean >/dev/null
echo -e "OK\n"
echo -e "                 Thats it!\n           I would give myself 20 points.\n              Have a nice day."
