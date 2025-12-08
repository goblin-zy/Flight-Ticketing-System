-- MySQL dump 10.13  Distrib 8.0.18, for Win64 (x86_64)
--
-- Host: localhost    Database: flight_booking_system
-- ------------------------------------------------------
-- Server version	8.0.18

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `order_change`
--

DROP TABLE IF EXISTS `order_change`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `order_change` (
  `change_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '改签记录ID',
  `order_id` varchar(50) NOT NULL COMMENT '订单ID',
  `old_flight_id` int(11) NOT NULL COMMENT '原航班ID',
  `new_flight_id` int(11) NOT NULL COMMENT '新航班ID',
  `price_difference` int(11) NOT NULL COMMENT '差价(分)',
  `change_time` datetime NOT NULL COMMENT '改签时间',
  PRIMARY KEY (`change_id`),
  KEY `fk_order_change_order_info` (`order_id`),
  KEY `fk_order_change_old_flight` (`old_flight_id`),
  KEY `fk_order_change_new_flight` (`new_flight_id`),
  CONSTRAINT `fk_order_change_new_flight` FOREIGN KEY (`new_flight_id`) REFERENCES `flight` (`flight_id`),
  CONSTRAINT `fk_order_change_old_flight` FOREIGN KEY (`old_flight_id`) REFERENCES `flight` (`flight_id`),
  CONSTRAINT `fk_order_change_order_info` FOREIGN KEY (`order_id`) REFERENCES `order_info` (`order_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='订单改签记录表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `order_change`
--

LOCK TABLES `order_change` WRITE;
/*!40000 ALTER TABLE `order_change` DISABLE KEYS */;
/*!40000 ALTER TABLE `order_change` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-12-08 17:46:43
