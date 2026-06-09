const MEDIA = "C:/Users/ssjjd/Desktop/TinyTensor/outputs/social_practice_report/extracted_media";
const FLOW = "C:/Users/ssjjd/Desktop/TinyTensor/outputs/manual-20260607-social-practice/presentations/social-practice-report/assets";

export const ASSET = {
  rmLogo: `${MEDIA}/image2.png`,
  orangeMark: `${MEDIA}/image1.png`,
  blueprint: `${MEDIA}/image6.jpeg`,
  city: `${MEDIA}/image7.jpeg`,
  robotHero: `${MEDIA}/image9.jpg`,
  robotClose: `${MEDIA}/image8.jpg`,
  runA: `${MEDIA}/image10.png`,
  runB: `${MEDIA}/image13.png`,
  runC: `${MEDIA}/image12.png`,
  camera: `${MEDIA}/image11_norm.png`,
  dualSim: `${MEDIA}/image14.png`,
  dualCad: `${MEDIA}/image15_norm.png`,
  singleCad: `${MEDIA}/image16.jpeg`,
  matlab: `${MEDIA}/image17.png`,
  clawV1: `${MEDIA}/image18_norm.png`,
  clawV2: `${MEDIA}/image19_norm.png`,
  clawV3: `${MEDIA}/image20_norm.png`,
  clawLatest: `${MEDIA}/image21.jpeg`,
  clawTable: `${MEDIA}/image22.jpg`,
  controllerV1: `${MEDIA}/image23.jpeg`,
  controllerV2: `${MEDIA}/image24.jpeg`,
  controllerV3: `${MEDIA}/image25.png`,
  controllerOpt: `${MEDIA}/image26_norm.png`,
  robotCad: `${MEDIA}/image27.jpg`,
  fieldArm: `${MEDIA}/image28.png`,
  yolo: `${MEDIA}/image29.png`,
};

const C = {
  bg: "#050B13",
  ink: "#08111F",
  panel: "#101C30CC",
  soft: "#14243A99",
  line: "#33506F",
  cyan: "#18BDF2",
  cyan2: "#7DE3FF",
  orange: "#FF7A1A",
  amber: "#FFC857",
  green: "#52D273",
  white: "#F8FBFF",
  muted: "#A9B6C8",
  dim: "#61728B",
};

const FONT = {
  title: "Microsoft YaHei UI",
  body: "Microsoft YaHei",
};

function t(ctx, slide, text, x, y, w, h, opts = {}) {
  return ctx.addText(slide, {
    text,
    x,
    y,
    width: w,
    height: h,
    typeface: opts.face || FONT.body,
    fontSize: opts.size || 22,
    color: opts.color || C.white,
    bold: opts.bold ?? false,
    align: opts.align || "left",
    valign: opts.valign || "top",
    name: opts.name,
    fill: opts.fill || "#00000000",
    line: opts.line || ctx.line("#00000000", 0),
    insets: opts.insets || { left: 0, right: 0, top: 0, bottom: 0 },
  });
}

function rect(ctx, slide, x, y, w, h, opts = {}) {
  return ctx.addShape(slide, {
    x,
    y,
    width: w,
    height: h,
    geometry: opts.geometry || "rect",
    fill: opts.fill || "#00000000",
    line: opts.line || ctx.line("#00000000", 0),
    name: opts.name,
  });
}

async function img(ctx, slide, path, x, y, w, h, opts = {}) {
  return ctx.addImage(slide, {
    path,
    x,
    y,
    width: w,
    height: h,
    fit: opts.fit || "cover",
    alt: opts.alt || "",
    name: opts.name,
  });
}

async function scene(ctx, slide, idx, opts = {}) {
  rect(ctx, slide, 0, 0, 1280, 720, { fill: C.bg });
  if (opts.image) {
    await img(ctx, slide, opts.image, opts.x || 0, opts.y || 0, opts.w || 1280, opts.h || 720, {
      fit: opts.fit || "cover",
      alt: opts.alt || "project visual",
    });
  }
  rect(ctx, slide, 0, 0, 1280, 720, { fill: opts.shade || "#050B13D9" });
  await img(ctx, slide, `${FLOW}/flow_${String(idx).padStart(2, "0")}.png`, 0, 0, 1280, 720, {
    fit: "cover",
    alt: "flowing trajectory overlay",
  });
  if (!opts.noFooter) {
    rect(ctx, slide, 0, 676, 1280, 44, { fill: "#050B13E8" });
    rect(ctx, slide, 0, 676, 1280, 3, { fill: C.cyan });
    t(ctx, slide, "双机械臂协作与力控制器系统 | 暑期社会实践汇报", 56, 689, 620, 20, {
      size: 12,
      color: C.muted,
    });
    t(ctx, slide, String(ctx.slideNumber).padStart(2, "0"), 1180, 689, 48, 20, {
      size: 12,
      color: C.cyan2,
      align: "right",
    });
  }
}

function claim(ctx, slide, kicker, title, x, y, w, opts = {}) {
  rect(ctx, slide, x, y + 4, 7, 30, { fill: opts.accent || C.cyan, name: `kicker-${ctx.slideNumber}-marker` });
  t(ctx, slide, kicker, x + 20, y, 350, 28, {
    size: 15,
    color: C.cyan2,
    bold: true,
    name: `kicker-${ctx.slideNumber}-label`,
  });
  t(ctx, slide, title, x, y + 42, w, opts.h || 112, {
    size: opts.size || 36,
    color: C.white,
    bold: true,
    face: FONT.title,
    name: `anim-title-${ctx.slideNumber}`,
  });
}

function glass(ctx, slide, x, y, w, h, opts = {}) {
  rect(ctx, slide, x, y, w, h, {
    geometry: "roundRect",
    fill: opts.fill || C.panel,
    line: ctx.line(opts.line || "#2B4768", opts.lineWidth || 1),
  });
}

function label(ctx, slide, text, x, y, w, opts = {}) {
  glass(ctx, slide, x, y, w, opts.h || 34, {
    fill: opts.fill || "#0C1B2CCC",
    line: opts.line || opts.color || C.cyan,
  });
  t(ctx, slide, text, x + 14, y + 7, w - 28, (opts.h || 34) - 8, {
    size: opts.size || 14,
    color: opts.color || C.cyan2,
    bold: true,
    align: opts.align || "center",
  });
}

function floatingText(ctx, slide, heading, body, x, y, w, opts = {}) {
  t(ctx, slide, heading, x, y, w, 34, {
    size: opts.headingSize || 24,
    color: opts.color || C.white,
    bold: true,
    face: FONT.title,
  });
  rect(ctx, slide, x, y + 42, opts.rule || 74, 2, { fill: opts.accent || C.orange });
  t(ctx, slide, body, x, y + 58, w, opts.bodyH || 68, {
    size: opts.bodySize || 16,
    color: opts.bodyColor || C.muted,
  });
}

async function photo(ctx, slide, path, x, y, w, h, opts = {}) {
  rect(ctx, slide, x - 6, y - 6, w + 12, h + 12, {
    fill: opts.ringFill || "#00000000",
    line: ctx.line(opts.line || C.cyan, opts.lineWidth || 1.5),
  });
  await img(ctx, slide, path, x, y, w, h, { fit: opts.fit || "cover", alt: opts.alt || "project source photo" });
  if (opts.caption) {
    t(ctx, slide, opts.caption, x, y + h + 9, w, 22, {
      size: 12,
      color: opts.captionColor || C.muted,
      align: "center",
    });
  }
}

function node(ctx, slide, no, text, x, y, opts = {}) {
  rect(ctx, slide, x, y, opts.r || 58, opts.r || 58, {
    geometry: "ellipse",
    fill: opts.fill || C.cyan,
    line: ctx.line("#FFFFFF66", 1),
    name: opts.name,
  });
  t(ctx, slide, no, x, y + 13, opts.r || 58, 26, {
    size: 19,
    bold: true,
    color: C.bg,
    align: "center",
    face: FONT.title,
  });
  t(ctx, slide, text, x - 56, y + (opts.r || 58) + 16, (opts.r || 58) + 112, 44, {
    size: 16,
    bold: true,
    color: C.white,
    align: "center",
  });
}

async function slide01(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 1, { image: ASSET.robotHero, shade: "#050B13B8", noFooter: true });
  await img(ctx, slide, ASSET.blueprint, 0, 0, 395, 720, { alt: "engineering blueprint" });
  rect(ctx, slide, 0, 0, 395, 720, { fill: "#008AD070" });
  rect(ctx, slide, 395, 0, 5, 720, { fill: C.cyan });
  await img(ctx, slide, ASSET.rmLogo, 62, 42, 165, 38, { fit: "contain", alt: "RoboMaster logo" });
  label(ctx, slide, "吴健雄学院 2026 暑期社会实践", 705, 58, 350, { fill: "#0C1B2C99" });
  t(ctx, slide, "深耕科技创新", 615, 152, 390, 42, { size: 30, color: C.cyan2, bold: true, face: FONT.title });
  t(ctx, slide, "双机械臂协作与\n力控制器系统", 615, 212, 570, 152, {
    size: 56,
    color: C.white,
    bold: true,
    face: FONT.title,
    name: "anim-title-1",
  });
  rect(ctx, slide, 615, 406, 390, 3, { fill: C.orange });
  t(ctx, slide, "社会实践汇报 | 建功“十五五” · 青春为中国式现代化挺膺担当", 615, 432, 560, 30, {
    size: 18,
    color: C.muted,
  });
  t(ctx, slide, "番思琦、王圣博、柴新恬、戚景程、彭嘉钰\n指导老师：王亮", 615, 530, 540, 60, {
    size: 18,
    color: C.white,
  });
  t(ctx, slide, "神奇妙妙社会实践队", 615, 620, 320, 26, { size: 18, color: C.cyan2, bold: true });
  return slide;
}

async function slide02(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 2, { image: ASSET.city, shade: "#050B13D6" });
  claim(ctx, slide, "PRACTICE POSITIONING", "把赛场工程问题，转化为科技创新社会实践。", 58, 54, 760);
  floatingText(ctx, slide, "活动主题", "建功“十五五” · 青春为中国式现代化挺膺担当", 92, 238, 360, { accent: C.cyan });
  floatingText(ctx, slide, "选题方向", "深耕科技创新，笃行产教融合。面向重点赛事和机器人产业需求开展揭榜攻关式学习实践。", 520, 284, 470, {
    color: C.cyan2,
    accent: C.orange,
    bodyH: 78,
  });
  glass(ctx, slide, 120, 505, 760, 82, { fill: "#071522CC", line: C.cyan });
  t(ctx, slide, "实践命题", 152, 526, 116, 24, { size: 17, color: C.orange, bold: true });
  t(ctx, slide, "验证双机械臂、力反馈控制器与视觉辅助抓取能否形成可落地的工程系统。", 280, 522, 560, 50, {
    size: 22,
    color: C.white,
    bold: true,
    name: "anim-proof-2",
  });
  label(ctx, slide, "校级答辩自陈约 4 分钟", 918, 532, 210, { color: C.orange, line: C.orange });
  return slide;
}

async function slide03(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 3, { image: ASSET.robotClose, shade: "#050B13C8" });
  rect(ctx, slide, 730, 0, 550, 720, { fill: "#050B1390" });
  await photo(ctx, slide, ASSET.robotClose, 790, 110, 390, 390, { caption: "RoboMaster 高并发装配场景", line: C.cyan });
  claim(ctx, slide, "WHY IT MATTERS", "任务升级后，单臂方案触到上限。", 58, 58, 660, { size: 40 });
  floatingText(ctx, slide, "赛事升级", "最高级任务要求同时装配两个能量单元，多线程与高并发成为硬约束。", 80, 236, 430, {
    accent: C.orange,
    bodySize: 17,
  });
  floatingText(ctx, slide, "双臂空白", "高自由度双臂对等协同仍缺少成熟落地路径。", 210, 386, 360, {
    color: C.cyan2,
    accent: C.cyan,
  });
  floatingText(ctx, slide, "感知盲区", "单向控制缺少末端接触力与装配状态反馈，容易盲操、掉矿或损伤结构。", 70, 518, 520, {
    color: C.white,
    accent: C.orange,
    bodyH: 64,
  });
  glass(ctx, slide, 750, 560, 420, 88, { fill: "#101C30DD", line: C.orange });
  t(ctx, slide, "核心判断", 780, 578, 110, 22, { size: 15, color: C.orange, bold: true });
  t(ctx, slide, "突破任务上限，要把机械结构、控制手感、视觉感知三线同步推进。", 780, 604, 350, 42, {
    size: 16,
    color: C.white,
    bold: true,
    name: "anim-proof-3",
  });
  return slide;
}

async function slide04(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 4, { shade: "#050B13F2" });
  claim(ctx, slide, "PRACTICE ARC", "实践是一条流动的工程闭环，而不是单点制作。", 58, 56, 840, { size: 38 });
  const pts = [
    ["01", "需求拆解", 106, 348, C.cyan],
    ["02", "单臂闭环", 286, 286, C.orange],
    ["03", "双臂构型", 475, 340, C.cyan],
    ["04", "夹爪迭代", 660, 270, C.orange],
    ["05", "力控交互", 846, 342, C.cyan],
    ["06", "视觉智能", 1034, 286, C.orange],
  ];
  for (const [no, text, x, y, fill] of pts) {
    node(ctx, slide, no, text, x, y, { fill, name: no === "01" ? "anim-proof-4" : undefined });
  }
  glass(ctx, slide, 156, 562, 900, 58, { fill: "#101C30D6", line: "#2B4768" });
  t(ctx, slide, "复用原答辩材料中的实物照片、仿真图、夹爪方案与控制器路线，重组为“调研-验证-迭代-转化”的汇报逻辑。", 190, 580, 832, 22, {
    size: 17,
    color: C.white,
    align: "center",
  });
  return slide;
}

async function slide05(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 5, { shade: "#050B13EF" });
  claim(ctx, slide, "PROOF OF BUILD", "单臂闭环先跑通，双臂实践才有可信起点。", 58, 52, 680, { size: 38 });
  await photo(ctx, slide, ASSET.runA, 740, 110, 390, 220, { caption: "一级兑换实测", line: C.cyan });
  await photo(ctx, slide, ASSET.runB, 870, 382, 280, 158, { caption: "二级兑换实测", line: C.orange });
  await photo(ctx, slide, ASSET.runC, 604, 392, 238, 150, { caption: "三级兑换实测", line: "#2B4768" });
  const metrics = [
    ["32s", "一级兑换", 72],
    ["23s", "二级兑换", 260],
    ["32s", "三级兑换", 448],
  ];
  for (const [value, labelText, x] of metrics) {
    t(ctx, slide, value, x, 260, 150, 56, {
      size: 42,
      color: value === "23s" ? C.orange : C.cyan2,
      bold: true,
      face: FONT.title,
      align: "center",
      name: value === "32s" && x === 72 ? "anim-proof-5" : undefined,
    });
    t(ctx, slide, labelText, x, 318, 150, 24, { size: 15, color: C.white, bold: true, align: "center" });
    rect(ctx, slide, x + 28, 352, 94, 2, { fill: value === "23s" ? C.orange : C.cyan });
  }
  floatingText(ctx, slide, "实物基础", "单臂系统已完成实物搭建与算法闭环，并具备赛场三级难度装配任务能力。", 78, 438, 450, {
    accent: C.orange,
    bodySize: 19,
    bodyH: 62,
  });
  return slide;
}

async function slide06(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 6, { shade: "#050B13F2" });
  claim(ctx, slide, "DUAL-ARM ARCHITECTURE", "对称同构与仿真验证，让落地先有确定性。", 58, 54, 760, { size: 38 });
  await photo(ctx, slide, ASSET.dualSim, 68, 230, 300, 230, { fit: "contain", caption: "空间布局", line: C.cyan });
  await photo(ctx, slide, ASSET.dualCad, 410, 174, 320, 270, { fit: "contain", caption: "双臂整体", line: C.orange });
  await photo(ctx, slide, ASSET.singleCad, 770, 250, 200, 210, { fit: "contain", caption: "关节标注", line: "#2B4768" });
  await photo(ctx, slide, ASSET.matlab, 1000, 160, 190, 210, { fit: "contain", caption: "MATLAB轨迹", line: C.cyan });
  floatingText(ctx, slide, "构型", "尺寸约束下完成双臂空间布局优化与图纸绘制。", 130, 520, 290, { bodySize: 15 });
  floatingText(ctx, slide, "轨迹", "完成核心装配动作连续轨迹规划与平滑性验证。", 500, 530, 330, { accent: C.cyan, bodySize: 15 });
  floatingText(ctx, slide, "落地", "为实车双臂联调扫清理论障碍。", 900, 510, 250, { accent: C.orange, bodySize: 15 });
  return slide;
}

async function slide07(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 7, { shade: "#050B13F0" });
  claim(ctx, slide, "END-EFFECTOR ITERATION", "夹爪从“能抓”，走到“抓得稳”。", 58, 54, 680, { size: 40 });
  await photo(ctx, slide, ASSET.clawV1, 76, 250, 165, 220, { fit: "contain", caption: "V1 实物", line: "#2B4768" });
  await photo(ctx, slide, ASSET.clawV2, 300, 206, 185, 242, { fit: "contain", caption: "V2 实物", line: C.cyan });
  await photo(ctx, slide, ASSET.clawV3, 548, 264, 172, 228, { fit: "contain", caption: "V3 方案", line: "#2B4768" });
  await photo(ctx, slide, ASSET.clawLatest, 820, 158, 280, 300, { fit: "contain", caption: "最新刚性圆弧夹爪", line: C.orange });
  t(ctx, slide, "关键改进", 116, 560, 140, 26, { size: 18, color: C.orange, bold: true });
  t(ctx, slide, "贴合道具形态的刚性圆弧夹持，面向复杂颠簸工况降低掉矿风险。", 256, 555, 760, 34, {
    size: 23,
    color: C.white,
    bold: true,
    name: "anim-proof-7",
  });
  return slide;
}

async function slide08(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 8, { shade: "#050B13F0" });
  claim(ctx, slide, "HAPTIC CONTROL", "人机交互从“下发指令”，升级到“触感闭环”。", 58, 54, 820, { size: 38 });
  await photo(ctx, slide, ASSET.controllerV1, 560, 154, 220, 172, { caption: "控制器 V1", line: "#2B4768" });
  await photo(ctx, slide, ASSET.controllerV2, 824, 128, 234, 184, { caption: "控制器 V2", line: C.cyan });
  await photo(ctx, slide, ASSET.controllerOpt, 600, 410, 220, 174, { caption: "V2 优化版", line: "#2B4768" });
  await photo(ctx, slide, ASSET.controllerV3, 872, 392, 218, 178, { fit: "contain", caption: "V3 图纸", line: C.orange });
  floatingText(ctx, slide, "路线一：V2 全电机版", "全关节电机驱动，加入重力补偿算法以优化手感，持续调优自重与稳定性。", 76, 250, 395, { accent: C.cyan, bodySize: 17 });
  floatingText(ctx, slide, "路线二：V3 腕部穿戴同构版", "碳纤维管连接，实现与机械臂及末端差速齿轮的同构映射，降低理解成本。", 76, 420, 425, { accent: C.orange, bodySize: 17 });
  rect(ctx, slide, 1090, 250, 126, 126, { geometry: "ellipse", fill: "#102740CC", line: ctx.line(C.cyan, 2) });
  t(ctx, slide, "所见\n即所得", 1092, 284, 122, 58, { size: 22, color: C.cyan2, bold: true, align: "center", name: "anim-proof-8" });
  rect(ctx, slide, 1090, 454, 126, 126, { geometry: "ellipse", fill: "#3A2112CC", line: ctx.line(C.orange, 2) });
  t(ctx, slide, "所触\n即所感", 1092, 488, 122, 58, { size: 22, color: "#FFD5B1", bold: true, align: "center" });
  return slide;
}

async function slide09(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 9, { shade: "#050B13F0" });
  claim(ctx, slide, "AI-ASSISTED GRASPING", "视觉智能让操作门槛继续下降。", 58, 54, 690, { size: 40 });
  await photo(ctx, slide, ASSET.yolo, 818, 148, 300, 300, { caption: "能量单元识别与实例分割", line: C.orange });
  const steps = [
    ["深度相机", 124, 300, C.cyan],
    ["YOLO-Segment", 322, 258, C.orange],
    ["ROS 2", 526, 304, C.cyan],
    ["MoveIt 2", 704, 256, C.cyan],
  ];
  for (const [text, x, y, color] of steps) {
    label(ctx, slide, text, x, y, 142, { color, line: color, fill: "#0B1A2BD6", h: 40, size: 14 });
  }
  floatingText(ctx, slide, "半自动抓取链路", "深度相机采集目标与空间信息，YOLO-Segment 完成实例分割，ROS 2 与 MoveIt 2 组织通信和轨迹规划。", 96, 470, 660, {
    accent: C.orange,
    bodySize: 18,
    bodyH: 64,
  });
  t(ctx, slide, "目标不是替代操作手，而是减少盲操、降低学习成本、提高装配稳定性。", 116, 590, 830, 28, {
    size: 21,
    color: C.white,
    bold: true,
    name: "anim-proof-9",
  });
  return slide;
}

async function slide10(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 10, { shade: "#050B13F2" });
  claim(ctx, slide, "OUTPUTS", "实践成果以四条可延展的能力沉淀下来。", 58, 54, 760, { size: 40 });
  const items = [
    ["硬件平台", "模块化双机械臂实体平台，兼顾高刚性、轻量化与整车适配。", 110, 244, C.cyan],
    ["交互系统", "自适应重力补偿与主动力反馈控制器，改善操作直觉。", 610, 220, C.orange],
    ["算法方案", "ROS 2 + YOLO + MoveIt2 的半自动辅助抓取路径。", 160, 470, C.green],
    ["实践材料", "立项申报、答辩汇报、过程记录与后续竞赛转化材料。", 700, 466, C.amber],
  ];
  for (const [head, body, x, y, color] of items) {
    rect(ctx, slide, x - 24, y + 7, 10, 60, { fill: color });
    t(ctx, slide, head, x, y, 260, 34, { size: 25, color: C.white, bold: true, face: FONT.title, name: head === "硬件平台" ? "anim-proof-10" : undefined });
    t(ctx, slide, body, x, y + 45, 400, 50, { size: 16, color: C.muted });
  }
  label(ctx, slide, "团队分工：策划组织 / 联络调研 / 数据整理 / 材料整合 / 媒体联系 / 成果分析", 200, 604, 860, {
    color: C.cyan2,
    line: "#2B4768",
    fill: "#101C30CC",
  });
  return slide;
}

async function slide11(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 11, { shade: "#050B13EF" });
  await photo(ctx, slide, ASSET.fieldArm, 780, 140, 350, 260, { caption: "工程实物与操作验证", line: C.cyan });
  claim(ctx, slide, "VALUE", "价值落在竞赛突破、工程转化与青年成长。", 58, 54, 760, { size: 38 });
  floatingText(ctx, slide, "01 赛事突破", "挑战双单元高频并发装配任务，延续团队 2025 全国一等奖的技术落地能力。", 86, 220, 560, { accent: C.cyan, bodySize: 17 });
  floatingText(ctx, slide, "02 工程转化", "双臂对等协同与力控夹爪可迁移到复杂异形件自适应抓取和工业自动化场景。", 86, 360, 600, { accent: C.orange, bodySize: 17 });
  floatingText(ctx, slide, "03 实践育人", "把理论学习、调研拆解、实物验证与团队协作落到真实工程问题中，练就过硬本领。", 86, 500, 610, { accent: C.green, bodySize: 17 });
  glass(ctx, slide, 778, 492, 352, 90, { fill: "#101C30D8", line: C.orange });
  t(ctx, slide, "安全底线", 808, 508, 120, 24, { size: 17, color: C.orange, bold: true });
  t(ctx, slide, "坚持安全第一，做好设备调试、电气联调、外出实践与赛场风险预案。", 808, 536, 290, 40, {
    size: 14,
    color: C.white,
    name: "anim-proof-11",
  });
  return slide;
}

async function slide12(presentation, ctx) {
  const slide = presentation.slides.add();
  await scene(ctx, slide, 12, { image: ASSET.robotHero, shade: "#050B13C8", noFooter: true });
  await img(ctx, slide, ASSET.blueprint, 0, 0, 390, 720, { alt: "blueprint visual" });
  rect(ctx, slide, 0, 0, 390, 720, { fill: "#008AD075" });
  rect(ctx, slide, 390, 0, 5, 720, { fill: C.cyan });
  t(ctx, slide, "NEXT STEPS", 520, 68, 260, 28, { size: 18, color: C.cyan2, bold: true });
  t(ctx, slide, "从单臂验证，迈向双臂整车联调。", 520, 122, 650, 56, {
    size: 40,
    bold: true,
    face: FONT.title,
    color: C.white,
    name: "anim-title-12",
  });
  const plan = [
    ["资金到位", "采购大扭矩电机与关键传动件"],
    ["整车联调", "复刻单臂闭环并完成双臂同步"],
    ["力控打磨", "优化重力补偿与主动力反馈"],
    ["赛场验证", "冲刺四级难度与国家级竞赛"],
  ];
  for (let i = 0; i < plan.length; i += 1) {
    const y = 242 + i * 80;
    rect(ctx, slide, 520, y, 54, 54, { geometry: "ellipse", fill: i === 0 ? C.orange : C.cyan });
    t(ctx, slide, String(i + 1), 520, y + 13, 54, 26, { size: 20, bold: true, color: C.bg, align: "center", face: FONT.title });
    t(ctx, slide, plan[i][0], 600, y + 2, 180, 26, { size: 21, bold: true, color: C.white });
    t(ctx, slide, plan[i][1], 600, y + 34, 440, 24, { size: 17, color: C.muted });
    if (i < plan.length - 1) rect(ctx, slide, 546, y + 58, 2, 22, { fill: "#31516D" });
  }
  rect(ctx, slide, 520, 604, 566, 2, { fill: C.orange });
  t(ctx, slide, "感谢垂听，敬请批评指正", 520, 626, 470, 42, { size: 30, bold: true, color: C.white, face: FONT.title });
  t(ctx, slide, "神奇妙妙社会实践队 | 指导老师：王亮", 520, 676, 400, 24, { size: 16, color: C.cyan2 });
  return slide;
}

const slides = [
  slide01,
  slide02,
  slide03,
  slide04,
  slide05,
  slide06,
  slide07,
  slide08,
  slide09,
  slide10,
  slide11,
  slide12,
];

export async function renderSlide(index, presentation, ctx) {
  return slides[index - 1](presentation, ctx);
}
