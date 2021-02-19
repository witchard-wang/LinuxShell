## echo <br>
echo “Hello world | Life is Good > Great $” <br>
echo ‘Hello world | Life is Good > Great $’ 


## normal commands
ls <br>
ls -l /sbin  <br>
ls -l -a <br>
ls -la  <br>
ps -aux <br>
ls -l ..  <br>
sleep 5 

## I/O redirect <br>
ps aux > a <br>
grep /init < a <br>
grep /init < a > b

## Single pipe <br>
ls | grep 'cpp' <br>

## Two or more pipe
ps aux | awk '/init/{print $1}' | sort -r <br>
ps aux | awk '/init/{print $1}' | sort -r | awk '/ro/' | grep ro <br>
ps aux | awk '{print $1$11}' | sort -r | grep root <br>

## Two or more pipe with I/O redirect <br>
ps aux > test.txt <br>
awk '{print $1$11}'< test.txt | head -10 | head -8 | head -7 | sort > out.txt <br>
cat out.txt <br>

## Background Processes <br>
sleep 1 & <br>
sleep 2 & <br>
sleep 20 & <br>

## Change Directory <br>
cd ../../ <br>
cd . <br>
cd /home/ <br>
cd -  <br>