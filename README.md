# ScrollAreaPlugin
虚幻引擎5.3.2，列表视图(ListView)，瓦片视图(TileView)，在PC端允许按住鼠标左键进行滚动视图，只需要在细节面版勾选bEnableLeftClickScrolling即可。
  
1.ScrollArea.zip 是已经完成编译的插件，编译版本为UE5.3.2发布版本。如果您是此版本可以直接使用，其他版本还未测试，可以自行尝试。
  
2.ScrollArea文件夹里面放置的是源码版本，如果编译版本不支持，可自行编译源码。
  
为什么使用ListView和TileView？它们都可以用有限的Widget通过滚动视图完成数万条的数据显示。

ScrollBox并不支持虚拟化，但是不代表不能为ScrollBox单独开发支持虚拟化的功能。
  
当然你依然可以使用ScrollBox但是ListView和TileView同样可以完成ScrollBox的任务，毕竟是现成的，除非您的项目真的需要自定义。