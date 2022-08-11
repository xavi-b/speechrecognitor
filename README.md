# Vosk dependency
apt install python3 ffmpeg
pip3 install vosk
pip3 install websockets
python3 -c 'from vosk import Model; model = Model(lang="fr")' # to download fr model
python3 -c 'from vosk import Model; model = Model(lang="de")' # to download de model
