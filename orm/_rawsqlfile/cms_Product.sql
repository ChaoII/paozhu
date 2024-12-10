CREATE TABLE `product` (
  `pid` int unsigned NOT NULL AUTO_INCREMENT,
  `userid` int unsigned NOT NULL,
  `topicid` int unsigned NOT NULL COMMENT '栏目id',
  `bigid` int unsigned NOT NULL COMMENT '大类',
  `smallid` int unsigned NOT NULL COMMENT '小类',
  `brandid` int unsigned NOT NULL COMMENT '品牌id',
  `isview` tinyint NOT NULL COMMENT '是否显示',
  `isstore` tinyint NOT NULL COMMENT '精品',
  `ishome` tinyint unsigned NOT NULL COMMENT '是否首页',
  `showtype` tinyint NOT NULL COMMENT 'page layout',
  `sntype` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '型号',
  `name` varchar(253) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '产品名称',
  `keywords` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `introduce` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '介绍',
  `listimg` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '列表图片',
  `bigimg` varchar(254) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '主图',
  `maincontent` longtext CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '产品主内容',
  `paracontent` longtext CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '参数',
  `samepro` varchar(254) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '类似',
  `attatchfiles` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '附件',
  `price` int unsigned NOT NULL COMMENT '价格',
  `sortid` int unsigned NOT NULL,
  `adddate` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '产品添加日期',
  `editdate` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '产品修改时间',
  PRIMARY KEY (`pid`),
  KEY `userid` (`userid`)
) ENGINE=InnoDB AUTO_INCREMENT=358 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci COMMENT='产品内容'