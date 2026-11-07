# MotionGloveUI Copilot 指引

本仓库是 Qt5 / C++14 的 CMake 界面库（产物 `MotionGloveUI.dll`）。以下记录「一键脚本」的
入口、调用链与维护约定：新增/改名/移动脚本时，必须同步更新本表，并同步修改调用方路径与
脚本内的 ROOT 推导层级。

**编译/测试必须遵守第八节的顺序：先在本机 WSL（Ubuntu-24.04）跑构建与测试，通过后再检查
GitHub Actions CI/CD 结果**，不要直接推远端等结果。

## 一、根目录入口

| 脚本 | 作用 | 常用命令 |
| --- | --- | --- |
| `buildDeploy.bat` | 双击入口：vswhere 检测 VS2022/2026（单套 3 秒倒计时直跑，多套弹菜单），随后清 `out\` 并串起构建 → 测试 → 文档 → 打包；任一步失败即停 | `buildDeploy.bat`、`buildDeploy.bat 2022 Release`、`buildDeploy.bat 2026 Debug` |

流水线：`[0/4]` 清 `out\` → `[1/4]` `scripts\build.bat <年份> [配置]` → `[2/4]` `scripts\test.bat [配置]` →
`[3/4]` `scripts\gen_docs.bat` → `[4/4]` `scripts\package.bat`。参数里带年份时跳过检测与菜单；
其余参数（Debug/Release）透传，年份不重复传递。

## 二、`scripts/` 用户入口

均为「双解释器」脚本：同一份 `.bat` 在 Windows 用 cmd 执行、在 Linux 用 `sh scripts/xxx.bat` 执行。

| 脚本 | 作用 | 常用命令 |
| --- | --- | --- |
| `scripts\build.bat` | 配置 + 编译 + 安装 + 部署：Windows 按年份选 VS 生成器（`2022` → `Visual Studio 17 2022`，`2026` → `Visual Studio 18 2026`），默认构建 Debug/Release 两份；Linux 用 CMake 默认生成器 | `scripts\build.bat 2022 Release`、`sh scripts/build.bat Release` |
| `scripts\deploy.bat` | 从构建树把 `MotionGloveUI` 的 `.dll`/`.lib`（Linux 为 `libMotionGloveUI.so*`/`.a`）与 `include\*.h` 汇总到 `out\install\x64-<配置>`；**源码树不再生成 `lib\`，也刻意不用 `cmake --install`**（会连带执行 CPM 依赖的 install 规则，把上游头文件/静态库装进 `out\install`） | `scripts\deploy.bat Debug [构建树]`、`sh scripts/deploy.bat Release` |
| `scripts\test.bat` | 独立构建树 `out\build\tdd-tests` 中构建**全部目标**（含 CPM 依赖注册的 `glovell_unit_tests`、`Cli.SelfCheck`，只编本项目两个目标会让它们 `Not Run` 并让 ctest 以 8 退出）后跑 ctest；第二参数是 `ctest -R` 正则 | `scripts\test.bat Release "Unit\..*"`、`sh scripts/test.bat Release Unit` |
| `scripts\gen_docs.bat` | Doxygen 文档：默认 `D:\autotools\doxygen-1.11.0\doxygen.exe`，可用 `DOXYGEN` 覆盖；告警即失败，日志归集到 `out\doxygen\doxygen_warnings.log`；Windows 成功后打开 `docs\html\index.html` | `scripts\gen_docs.bat`、`sh scripts/gen_docs.bat` |
| `scripts\package.bat` | 打可分发源码 ZIP，输出到 `out\pack\MotionGloveUI_v<版本>.zip`；版本号缺省取 `Doxyfile` 的 `PROJECT_NUMBER` | `scripts\package.bat [版本号]`、`sh scripts/package.bat` |
| `scripts\git_pull_latest.bat` | 从 `origin/main` 拉取最新代码并打印最近 5 条提交 | `scripts\git_pull_latest.bat`、`sh scripts/git_pull_latest.bat` |
| `scripts\open_in_vscode.bat` | 用 VSCode 打开工程根；缺少 `code` 命令时明确报错 | `scripts\open_in_vscode.bat`、`sh scripts/open_in_vscode.bat` |
| `scripts\release.bat` | 从 `Doxyfile` 的 `PROJECT_NUMBER` 取版本，校验工作树干净与 tag 不冲突后创建并推送 annotated tag `v<版本>` | `scripts\release.bat [--force]`、`sh scripts/release.bat` |

通用环境变量：`QTDIR`（Qt5 前缀，传给 `CMAKE_PREFIX_PATH`）、`XSTYPES_FETCH_VIA_SSH`（私有 CPM 依赖走
HTTPS 还是 SSH，默认 `OFF`）、`DOXYGEN`（doxygen 路径）。

## 三、`scripts/ci/` 远程 CI 辅助

`.bat` 只是转发参数并透传退出码，实现全在同目录 `.ps1`；公共的 gh 路径 / 令牌 / 仓库解析在
`ci_common.ps1`（`GH_CLI`、`GH_TOKEN`、`GH_REPO_DEFAULT` 可覆盖）。

| 脚本 | 作用 | 常用命令 |
| --- | --- | --- |
| `check_ci.bat` | 列出运行 / `-Watch` 等待 / `-RunId` 查看失败日志 | `scripts\ci\check_ci.bat -Watch -Workflow cmake-multi-platform.yml` |
| `rerun_failed.bat` | 只重跑某次运行的失败 job，破坏性动作支持 `-DryRun` | `scripts\ci\rerun_failed.bat -RunId <id> -DryRun` |
| `pull_artifacts.bat` | 下载运行产物到 `out\ci-artifacts`（下载前清空，支持 `-Name`/`-Dest`） | `scripts\ci\pull_artifacts.bat -RunId <id>` |

## 四、`scripts/wsl/` 本地 WSL 预验证

开发专用，不进客户包、不进 CI。入口 `wsl.bat` 把 Windows 路径转 `/mnt/...`，并显式传
`MOTIONGLOVE_UI_WSL_SOURCE` 避免副本「自我同步」；任务脚本只做「同步 + 转发」，构建逻辑仍只在
`scripts/{build,test,gen_docs}.bat` 中维护。

| 命令 | 作用 |
| --- | --- |
| `scripts\wsl\wsl.bat sync` | rsync 到副本（默认 `~/MotionGloveUI`，排除 `out`/`lib`/`.git`/`docs/html`） |
| `scripts\wsl\wsl.bat build Release` | 副本内跑 `sh scripts/build.bat Release` |
| `scripts\wsl\wsl.bat test Release <正则>` | 副本内跑测试，正则透传 |
| `scripts\wsl\wsl.bat docs` | 副本内用 WSL 自带 doxygen 复验文档门禁 |
| `scripts\wsl\wsl.bat verify` | build → test → docs，任一失败即停（无参数默认） |

可覆写：`WSL_DISTRO`（默认 `Ubuntu-24.04`）、`WSL_DEST`（默认 `~/MotionGloveUI`）、`WSL_QTDIR`；
日志固定写 `~/wsl_last.log`。本机发行版默认用户为 `rmpro`，需要 `sudo` 装包时由使用者在执行时
提供凭据，**不要把用户名/密码/令牌写进仓库文件**。

Linux 侧编译前置条件：系统 Qt5（`qtbase5-dev` + **`qtmultimedia5-dev`**，缺后者会报
`QtMultimedia/QMediaPlayer: No such file or directory`）、cmake、g++、git、rsync。私有 CPM 依赖
（CarbonUI / MotionGloveLLSDK）需要 GitHub 令牌：优先用 `GIT_CONFIG_COUNT=1` +
`GIT_CONFIG_KEY_0=url.https://x-access-token:<token>@github.com/.insteadOf` +
`GIT_CONFIG_VALUE_0=https://github.com/` 以环境变量注入，**不要写入 git config 或脚本**。

## 五、内部子脚本（不进用户入口表）

| 脚本 | 作用 |
| --- | --- |
| `scripts\lib\build_and_test.bat` | Windows 测试链：vswhere 探测 VS、由 `installationVersion` 主版本推导生成器、加载 `vcvars64.bat`、配置/构建测试目标、跑 ctest |
| `scripts\lib\package_source.ps1` | 唯一的打包实现：排除内部目录、泄漏校验、robocopy/7z 跨平台回退（无 7z 时用 `Compress-Archive`） |

只被其它脚本调用的子脚本一律放 `scripts\lib\`、`scripts\ci\`、`scripts\wsl\` 等子目录，不要在
`scripts\` 根新增内部工具。

## 六、改动脚本时必须遵守

- `.bat`/`.cmd` 必须 **CRLF、无 BOM**；`.sh`/`.ps1` 必须 **LF**，由根目录 `.gitattributes` 锚定。
- 双解释器脚本首行固定为 `:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #`（行尾 ` #` 吃掉 CR 不能省），第 2 行 `@echo off`、第 3 行 `chcp 65001 >nul`（必须在任何中文之前），cmd 段末尾 `exit /b` 后再放 `#__SH__` 与 POSIX 段。
- ROOT 推导按层级：根目录脚本 `%~dp0.` / `dirname "$0"`，`scripts\` 层 `%~dp0..` / `dirname "$0"/..`，两层子目录再多一级 `..`。
- cmd 的 `echo` 内容里禁止裸 `<` `>`，箭头写成 `-^>`；括号块内刚自增的变量用 `!VAR!` 延迟展开；不要用 `catalog_productLineVersion` 推生成器。
- 每步用 `if errorlevel 1 goto :failed` 之类的检查把退出码传出去；`ctest` 零用例必须判失败（先 `ctest -N` 断言 `Total Tests: [1-9]`），不能只看返回 0。
- 生成物只进 `out\`（构建树 `out\build\`、安装 `out\install\`）；库产物不再写源码树 `lib\`，
  由 `deploy.bat` 从构建树汇总进 `out\install\x64-<配置>\lib`，不要改回 `cmake --install`
  （会连带执行 CPM 依赖的 install 规则污染安装目录）。打包脚本排除 `out/`、`lib/`、`.git*`、`.github/`、`.codebuddy/`、`docs/html`、`scripts/ci`、`scripts/wsl`、`git_pull_latest.bat`、`release.bat`、`AGENTS.md`、`CMakeUserPresets.json`。新增内部脚本或 README 章节时，先确认不会混进客户包，必要时同步更新排除清单。
- 改完至少各冒烟一次 cmd 与 sh 两条分支；`scripts\test.bat` 要真在 cmd 里跑一次，才能覆盖深层子脚本的 CRLF/chcp 解析。

## 七、已废弃入口

根目录的 `[Windows]git_pull_latest.cmd`、`[Windows]open_in_vscode.bat`、`[Linux]git_pull_latest.sh`、
`[Linux]open_in_vscode.sh` 已删除；不要再引用它们，对应功能分别由 `scripts\git_pull_latest.bat` 和
`scripts\open_in_vscode.bat` 的双解释器分支提供。

## 八、编译测试与 CI/CD 顺序（必守）

1. **先本机 WSL**：发行版 `Ubuntu-24.04`、用户 `rmpro`，按第四节跑
   `scripts\wsl\wsl.bat build Release` → `scripts\wsl\wsl.bat test Release`（或 `verify` 加文档门禁）。
   构建/测试树在 WSL 家目录副本里，与 Windows 的 `out\` 隔离；Linux 侧全绿才算本地通过。
2. **再提交/推送**：本地通过后提交改动并 `git push origin main`，由
   `.github/workflows/cmake-multi-platform.yml` 触发 CI。
3. **最后查 CI/CD**：`scripts\ci\check_ci.bat -Limit 5` 找本次运行，`-RunId <id>` 看失败日志，
   `-Watch` 等结论；自动化环境里 `gh run watch` 长时间无输出会被判空闲，改用
   `gh run view <id> --json status,conclusion` 分段轮询。失败 job 可用
   `scripts\ci\rerun_failed.bat -RunId <id> -DryRun` 预演。

CI 的 runner 标签固定为 **`ubuntu-24.04`**（与本机 WSL 同版本），不要用 `-latest` 或
`ubuntu-22.04`。改 `matrix.os` 时必须同步 `include` 里的 `os:` 与所有
`if: matrix.os == 'ubuntu-24.04'` 条件，否则装依赖/装 Qt 步骤会被静默跳过。
矩阵只保留 `build_shared: [ON]`（发布形态是动态库，只测共享库），静态库组合留给本地构建；
每轮 job 数固定为 2。官方 Action 固定用 Node.js 24 主版本
（`actions/checkout@v6`、`actions/cache@v6`、`actions/setup-python@v6`），避免出现
`Node.js 20 is deprecated` 警告；`jurplel/install-qt-action@v4` 是 composite，无需升级。

排障提示：CPM 依赖按 `GIT_TAG main` 拉取，用例数会随上游变化。曾经 `MotionGloveLLSDK`
新增的 `Cli.SelfCheck`（`motionglovellsdk-cli --json --self-check`，TIMEOUT 30）只在 Windows
runner 挂起、Linux 瞬时通过，导致两个 Windows job 红灯。本仓库在 `CMakeLists.txt` 的
MotionGloveLLSDK `CPMAddPackage` 里加了 `OPTIONS "GLOVE_HMAX_SDK_BUILD_CLI OFF"`：本工程只链接
SDK 库，不需要它的命令行前端，从注册源头移除该用例（SDK 自身的 GoogleTest 用例仍照常参与
ctest，共 108 个）。上游 Windows 挂起仍需在 SDK 侧修复；**不要**用 `ctest -E Cli.SelfCheck`
在 workflow 里掩盖，也不要因此改本仓库脚本。
