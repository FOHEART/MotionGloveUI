# MotionGloveUI Copilot 指引

本仓库是 Qt5 / C++14 的 CMake 界面库（产物 `MotionGloveUI.dll`）。以下记录「一键脚本」的
入口、调用链与维护约定：新增/改名/移动脚本时，必须同步更新本表，并同步修改调用方路径与
脚本内的 ROOT 推导层级。

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
| `scripts\deploy.bat` | `cmake --install` 到 `out\install\x64-<配置>`，并汇总 `include\*.h` 与源码树 `lib\<配置>` 下的 `.lib`/`.dll`（Linux 为 `libMotionGloveUI.so*`） | `scripts\deploy.bat Debug [构建树]`、`sh scripts/deploy.bat Release` |
| `scripts\test.bat` | 独立构建树 `out\build\tdd-tests` 中构建 `h2ui_unit_tests`、`h2ui_ui_tests` 并跑 ctest；第二参数是 `ctest -R` 正则 | `scripts\test.bat Release "Unit\..*"`、`sh scripts/test.bat Release Unit` |
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

可覆写：`WSL_DISTRO`（默认 `Ubuntu-24.04`）、`WSL_DEST`、`WSL_QTDIR`；日志固定写 `~/wsl_last.log`。

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
- 生成物只进 `out\`（例外：CMakeLists 把库产物输出到源码树 `lib\<配置>`，由 `deploy.bat` 汇总）；打包脚本排除 `out/`、`lib/`、`.git*`、`.github/`、`.codebuddy/`、`docs/html`、`scripts/ci`、`scripts/wsl`、`git_pull_latest.bat`、`release.bat`、`AGENTS.md`、`CMakeUserPresets.json`。新增内部脚本或 README 章节时，先确认不会混进客户包，必要时同步更新排除清单。
- 改完至少各冒烟一次 cmd 与 sh 两条分支；`scripts\test.bat` 要真在 cmd 里跑一次，才能覆盖深层子脚本的 CRLF/chcp 解析。

## 七、已废弃入口

根目录的 `[Windows]git_pull_latest.cmd`、`[Windows]open_in_vscode.bat`、`[Linux]git_pull_latest.sh`、
`[Linux]open_in_vscode.sh` 已删除；不要再引用它们，对应功能分别由 `scripts\git_pull_latest.bat` 和
`scripts\open_in_vscode.bat` 的双解释器分支提供。
