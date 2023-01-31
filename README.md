# Privileger

![logo](https://user-images.githubusercontent.com/92790655/215750071-a79364b8-9331-4552-9aac-bb1aa9214e02.png)


## RU
Privileger позволяет вам максимально просто работать с привилегиями в ОС Windows. Присутствует три режима:
1. Добавить привилегии аккаунту. Реализовано через взаимодействие с LSA посредством функции LSA `LsaAddAccountRights()`:
![Добавление привилегий](https://user-images.githubusercontent.com/92790655/215747678-f17571bf-9748-47e1-9d81-e8c79b44adbd.png)

2. Запустить процесс, добавив в его токен конкретную привилегию. Реализовано с помощью функций `ImpersonateSelf()`, `OpenThreadToken()`. `AdjustTokenPrivileges()`, `CreateProcessWithTokenW()`:

![Стандартный процесс без нужной привилегии](https://user-images.githubusercontent.com/92790655/215748032-aab74846-5766-48d2-9510-d5140118117b.png)

![Успешное получение нужной привилегии](https://user-images.githubusercontent.com/92790655/215748148-47382349-b0e6-4ae1-8aa9-955eb6db8abc.png)

3. Удалить у пользователя привилегию. Осуществляется через взаимодействие с LSA посредством функции LSA `LsaRemoveAccountRights()`:

![Успешное удаление привилегии](https://user-images.githubusercontent.com/92790655/215748332-621efaa0-20ba-4809-b0c4-23dcd0f0963d.png)


## EN
Privileger allows you to work with privileges in Windows as easily as possible. There are three modes:
1. Add privileges to an account. Implemented through interaction with the LSA via the LSA function `LsaAddAccountRights()`:
![Adding privileges](https://user-images.githubusercontent.com/92790655/215747678-f17571bf-9748-47e1-9d81-e8c79b44adbd.png)

2. Start a process by adding a specific privilege to its token. Implemented by `ImpersonateSelf()`, `OpenThreadToken()`. `AdjustTokenPrivileges()`, `CreateProcessWithTokenW()`:

![Standard process without the required privilege](https://user-images.githubusercontent.com/92790655/215748032-aab74846-5766-48d2-9510-d5140118117b.png)

![Successful obtaining of the desired privilege](https://user-images.githubusercontent.com/92790655/215748148-47382349-b0e6-4ae1-8aa9-955eb6db8abc.png)

3. Remove privilege from the user. Performed through interaction with the LSA via the LSA `LsaRemoveAccountRights()` function:

![Successful removal of a privilege](https://user-images.githubusercontent.com/92790655/215748332-621efaa0-20ba-4809-b0c4-23dcd0f0963d.png)

