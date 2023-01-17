# Minuterie pour planeur de vol libre
<img align="left" width="80" height="80" src="https://github.com/fra589/WeTimer/blob/master/doc/images/WeTimer_logo.svg">
Basée sur une carte microcontrôleur WiFi ESP8266 de type Wemos D1 mini (34mm x 26mm), la minuterie WeTimer permet de commander deux servos en fonction des différentes phases de vol du planeur.
L’un des servo permet de commander les mouvement du stabilisateur avec une position pendant la phase de treuillage, une position pendant la phase de vol et une dernière position lors du déthermalisage. L’autre servo permet 2 positions de dérive, l’une pendant le treuillage lorsque le câble est tendu (dérive dans l’axe), l’autre pendant le vol ou lorsque le câble est détendu. Le temps de vol débute suite à une action sur un switch actionné par la tension ou le largage du câble de
treuillage.
La configuration se fait au travers d'un interface web accessible via le WiFi.
La notice de cette minuterie est ici : https://github.com/fra589/WeTimer/blob/master/doc/Notice%20WeTimer%20v0.9.4.pdf
