# Vosk dependency
pip3 install vosk
pip3 install websockets
apt install ffmpeg
touch test.mp4
vosk-transcriber -l en -i test.mp4 -o test.txt # to download en model, will fail since test.mp4 is invalid
vosk-transcriber -l fr -i test.mp4 -o test.txt # to download fr model, will fail since test.mp4 is invalid
rm test.mp4
rm test.txt
