# 📚 NJUPT Library Management System 

> 一个基于 Qt 5.14.2 (C++) 开发的桌面端图书馆管理系统，专为NJUPT课程设计打造。支持管理员与读者双角色登录，具备完整的图书借阅、归还、续借、预借及逾期管理功能。

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)![Qt](https://img.shields.io/badge/Qt-5.14.2-41cd52)![Compiler](https://img.shields.io/badge/MinGW-7.3.0_64bit-blue)![Platform](https://img.shields.io/badge/Platform-Windows-0078d7)

---

## ✨ 项目亮点 (Features)

本项目采用 **Qt Widgets** 技术栈开发，界面通过 QSS 美化，结合文件 I/O 实现数据持久化。

### 👥 角色权限体系
| 功能 | 管理员 (Admin) | 读者 (Reader) |
| :--- | :---: | :---: |
| **图书检索** | ✅ | ✅ |
| **借阅/归还** | ✅ | ✅ |
| **续借图书** | ❌ (逻辑上通常由读者操作) | ✅ (支持续借) |
| **图书预定** | ❌ | ✅ (支持预借/取消) |
| **图书上架/下架** | ✅ | ❌ |
| **修改图书信息** | ✅ | ❌ |
| **查看逾期报表** | ✅ | ❌ |
| **数据导出** | ✅ | ❌ |

### 🛠️ 核心业务逻辑
- [x] **智能索引**：自动根据类别生成索引号（如 `A0005`）。
- [x] **借阅限制**：每位读者最多借阅 **20本** 书，借期 **30天**。
- [x] **逾期风控**：若有逾期未还图书，系统自动禁止该读者继续借阅。
- [x] **续借与预定**：支持图书续借（重置归还日期）及热门图书预定功能。
- [x] **状态管理**：图书状态自动流转（可借 -> 已借出 -> 预定中）。
- [x] **数据持久化**：所有图书数据 (`books.txt`) 和用户数据 (`users.txt`) 本地文本存储。

---


## 📖 使用指南 (Usage)

### 1. 登录系统
系统启动后，首先进入登录界面。根据你的角色选择不同的登录方式：

*   **管理员登录 (Admin)**
    *   **账号**: `admin`
    *   **密码**: `123` (默认)
    *   *权限：图书上架、修改、下架、代借、代还、查看逾期报表及数据导出。*

*   **读者登录 (Reader)**
    *   **账号**: 直接输入任意**非 admin** 的用户名（例如 "student1"）。
    *   **密码**: `123` (首次登录系统会自动注册该账号并设置此默认密码)。
    *   *权限：图书检索、借阅、归还、续借及预借。*

### 2. 功能操作
*   **修改密码**: 登录界面点击“修改密码”按钮，验证旧密码后即可更新。
*   **图书查询**: 支持模糊搜索，在主界面输入书名关键词即可筛选。
*   **借阅规则**: 每位读者最多借阅 **20** 本书，期限 **30** 天。若有逾期未还图书，系统将锁定借阅功能。

---

## 📂 项目结构 (Structure)

```text
njupt_library_management/
├── njupt_library_management.pro  # Qt 项目配置文件
├── main.cpp                      # 程序入口 (处理登录循环逻辑)
├── MainWindow.h/cpp              # 主窗口 (UI交互、业务功能调度)
├── LoginDialog.h/cpp             # 登录/注册模块 (账号验证、文件读写)
├── LibraryManager.h/cpp          # 核心业务控制器 (图书增删改查、借还算法)
├── Book.h/cpp                    # 图书实体类 (数据模型、序列化)
├── res.qrc                       # Qt 资源配置文件
├── fig/                          # 图片资源文件夹 (logo, 背景图)
├── books.txt                     # [自动生成] 图书数据存储文件
└── users.txt                     # [自动生成] 用户账号存储文件

## 🚀 快速开始 (Getting Started)

### 1. 下载发行版 (For Users)
如果你只想运行程序，请直接前往 [Releases 页面](https://github.com/jpdsg/NJUPT_library_management_system/releases) 下载最新的 `.zip` 压缩包。
1. 解压压缩包。
2. 确保文件夹内包含 `.exe` 及所有依赖的 `.dll` 文件。
3. 双击运行 `njupt_library_management.exe`。

### 2. 编译构建 (For Developers)

#### 开发环境
*   **IDE**: Qt Creator
*   **Qt Version**: 5.14.2
*   **Compiler**: MinGW 7.3.0 64-bit
*   **OS**: Windows 10/11

#### 编译步骤
1. 克隆仓库：
   ```bash
   git clone https://github.com/你的用户名/njupt_library_management.git

