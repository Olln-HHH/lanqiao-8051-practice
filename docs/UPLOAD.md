# 上传GitHub

## 网页上传

1. 在GitHub新建空仓库，建议名称lanqiao-8051-practice。
2. 选择Add file → Upload files。
3. 把本仓库根目录里的文件和文件夹上传，确保README.md位于仓库根目录。
4. 不要只上传整个ZIP；ZIP适合作为Release附件。

## Git上传

在本仓库根目录打开终端，执行：

```powershell
git init -b main
git add .
git commit -m "整理51模块练习与历年真题"
git remote add origin 你的GitHub仓库HTTPS地址
git push -u origin main
```

把“你的GitHub仓库HTTPS地址”替换为新仓库的真实地址。Git身份和登录由你自己的账号配置。

仓库描述建议：蓝桥杯51单片机模块练习、应用练习与历年真题实践，附Keil工程、修复记录和编译固件。

Topic建议：lanqiao、8051、stc15、embedded-c、keil。获得真实上板反馈后，再更新对应例程的验证状态；B站视频发布后可把真实链接补到对应README。
