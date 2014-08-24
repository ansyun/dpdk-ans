

#ifndef __NETDP_LOG_H__
#define __NETDP_LOG_H__


#define NETDP_DEBUG( fmt, args...) \
	RTE_LOG(DEBUG, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_INFO( fmt, args...) \
	RTE_LOG(INFO, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_NOTICE( fmt, args...) \
	RTE_LOG(NOTICE, USER8, "%s(): " fmt "\n", __func__, ## args)


#define NETDP_WARNING( fmt, args...) \
	RTE_LOG(WARNING, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_ERR( fmt, args...) \
	RTE_LOG(ERR, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_CRIT( fmt, args...) \
	RTE_LOG(CRIT, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_ALERT( fmt, args...) \
	RTE_LOG(ALERT, USER8, "%s(): " fmt "\n", __func__, ## args)

#define NETDP_EMERG( fmt, args...) \
	RTE_LOG(EMERG, USER8, "%s(): " fmt "\n", __func__, ## args)


#endif /* __NETDP_LOG_H__ */
