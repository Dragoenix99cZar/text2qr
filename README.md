# text2qr

### Fork from

```
https://github.com/fukuchi/libqrencode
```

```
https://fukuchi.org/en/works/qrencode/
```

```
https://fukuchi.org/works/qrencode/manual/index.html
```


### Build instruction
- For libqrencode

cd in **libqrencode\build**
```
cmake ".." -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_RC_COMPILER=windres -DWITH_TOOLS=NO -DWITHOUT_PNG=YES
```

- For app(Win) (w64devkit)
```
gcc text2qr.c -o text_to_qr.exe -I..\libqrencode -L..\libqrencode\builds -lqrencode
```


![Github Link](qrcode.jpg "My Repo")
