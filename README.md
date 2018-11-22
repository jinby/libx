##LibX是什么?

本库为`开源库`，欢迎大家各种吐槽。本库为 [C.S.T](http://bbs.0xcode.org "打开C.S.T论坛") 整理发布。

本库`不作为商业`及其他`收费服务`，代码多为网友共享代码。如有触犯，请`吐槽`，将会对应删除。
    
开源地址：https://code.csdn.net/kingsollyu/libx

##更新日志：

每次更新都会写在这里，但这里只显示前三次的更新日志，更多请参考 [update.log](https://code.csdn.net/kingsollyu/libx/tree/master/update.log)

###2013.10.17

  * `[!]`修改`String`中`string_replace`函数，修改为返回替换次数。
  * `[+]`在`String`中增加对`U码`加密解密支持。（个人代码，不全面。）
  * `[+]`增加了写常用的功能函数，不再赘述。
  * `[~]`将`CLibX::Update::getWebSource`的参数从 `lstring` 改为了 `LPCTSTR`。

###2013.10.16

  * `[!]`将原来的`Window`类库改名为`Item`这样更贴切。
  * `[!]`转`Reg`类和`ini`类整合到`Config`类库中，增加一些常用功能函数。
  * `[!]`去掉鸡肋库`Encoder`，将加密功能放在`String`类库中。
  * `[+]`增加`String`类库，此库为常用字符串操作，简单字符串加密\解密功能。
  * `[+]`File 类中增加了几个常用函数;
  * `[+]`移植`System`类库并增加了一些常用功能函数。
  * `[+]`增加`Update`类库，此库可以用来为程序做简单升级。

###2013.10.14

  * `[!]`原来的LibX类名改为CLibX
  * `[!]`基本完全重构，去掉CString，改用std::string
  * `[!]`好吧，去掉了错别字。
  * `[-]`去掉Process这个类库，太鸡肋了只有两个函数。被移到Memory类库中了。
  * `[-]`去掉Http类，如果想使用网络，可以使用开源的`Curl`库。
  * `[+]`File 类中增加了几个常用函数;

##关于作者
  * [King.Sollyu](http://blog.csdn.net/kingsollyu "我的CSDN博客地址")