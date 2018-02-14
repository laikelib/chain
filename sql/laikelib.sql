#!/bin/bash

CREATE DATABASE IF NOT EXISTS laikelib_db;
USE laikelib_db;


DROP TABLE IF EXISTS `t_blocks`;
CREATE TABLE `t_blocks` (
    `id` INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `hash` VARCHAR(256),
    `height` INT(11),
    `version` INT(11),
    `type` INT(11),
    `hashPrev` VARCHAR(256),
    `hashMerkle` VARCHAR(256),
    `blockTime` INT(24),
    `bits` INT(24),
    `count` INT(11),
    `nonce` INT(11),
    `createBy` VARCHAR(64),
    `amount` BIGINT,
    `createTime` DATETIME,
    `modifyTime` DATETIME) ENGINE=InnoDB DEFAULT CHARSET=utf8;
     
DROP TABLE IF EXISTS `t_txs`;
CREATE TABLE `t_txs` (
       `id` INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
       `hash` VARCHAR(256),
       `height` INT(11),
       `type` INT(11),
       `tranTime` INT(24),
       `sender` VARCHAR(64),
       `receiver` VARCHAR(64),
       `amount` BIGINT,
       `fee` BIGINT,
       `blockHash` VARCHAR(256),
       `createTime` DATETIME,
       `modifyTime` DATETIME) ENGINE=InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `t_config`;
CREATE TABLE `t_config` (
    `id` INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `pid` INT(11),
    `key` VARCHAR(64),
    `value` VARCHAR(256),
    `n1` INT(11),
    `n2` INT(11),
    `s1` VARCHAR(64),
    `s2` VARCHAR(128),
    `d` DATETIME,
    `bstate` TINYINT,
    `createTime` DATETIME,
    `modifyTime` DATETIME
    ) ENGINE =InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `t_accounts`;
CREATE TABLE `t_accounts` (
    `id` INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `address` VARCHAR(64),
    `publicKey` VARCHAR(256),
    `balance` BIGINT,
    `inCount` INT(11),
    `outCount` INT(11),
    `createTime` DATETIME,
    `modifyTime` DATETIME) ENGINE = InnoDB DEFAULT CHARSET = utf8;
