# Ubuntu 24.04 MySQL 安裝與客戶端工具指南

## MySQL Server 安裝

### 1. 更新系統套件
```bash
sudo apt update
sudo apt upgrade
```

### 2. 安裝 MySQL Server
```bash
sudo apt install mysql-server
```

### 3. 啟動並啟用 MySQL 服務
```bash
sudo systemctl start mysql
sudo systemctl enable mysql
```

### 4. 執行安全設定
```bash
sudo mysql_secure_installation
```

安全設定會詢問以下選項：
- 設定 root 密碼
- 移除匿名使用者
- 禁止 root 遠端登入
- 移除測試資料庫
- 重新載入權限表

## MySQL 客戶端工具推薦

### 命令列工具

#### MySQL Client (官方命令列工具)
```bash
# 安裝
sudo apt install mysql-client

# 使用
mysql -u username -p
mysql -h hostname -u username -p database_name
```

#### mycli (改良版命令列工具)
```bash
# 安裝
sudo apt install mycli

# 使用 (支援自動完成和語法高亮)
mycli -u username -p
```

### 圖形化界面工具

#### 1. MySQL Workbench (官方推薦)
```bash
# 安裝
sudo apt install mysql-workbench
```

**特色功能：**
- 官方開發，功能完整
- 資料庫設計與建模
- SQL 開發與執行
- 伺服器管理
- 資料匯入/匯出
- 視覺化 ER 圖設計

#### 2. DBeaver Community Edition (免費、功能豐富)
```bash
# 方法 1: Snap 安裝
sudo snap install dbeaver-ce

# 方法 2: 下載 .deb 套件
wget https://dbeaver.io/files/dbeaver-ce_latest_amd64.deb
sudo dpkg -i dbeaver-ce_latest_amd64.deb
sudo apt-get install -f
```

**特色功能：**
- 支援多種資料庫 (MySQL, PostgreSQL, SQLite 等)
- 