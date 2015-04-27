// Standard
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Tools
#include <MsgLogger.h>
#include <epicsTimeHelper.h>
// local
#include "ArchiveDataClient.h"

ArchiveDataClient::ArchiveDataClient(const char *url) //: URL(url)
{
    // W/o this call, we can't get more than ~ 4 x 300 values
    //xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 10000*1024);
	strcpy(URL, url);
    xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 1000000*1024);
    xmlrpc_client_init(XMLRPC_CLIENT_NO_FLAGS, "ArchiveDataClient", "1");
    xmlrpc_env_init(&env);
}

ArchiveDataClient::ArchiveDataClient()
{
    xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 1000000*1024);
    xmlrpc_client_init(XMLRPC_CLIENT_NO_FLAGS, "ArchiveDataClient", "1");
    xmlrpc_env_init(&env);
}

ArchiveDataClient::~ArchiveDataClient()
{
    xmlrpc_env_clean(&env);
    xmlrpc_client_cleanup();
}

bool ArchiveDataClient::getInfo(int &version,
                                stdString &description,
                                stdVector<stdString> &how_strings,
                                stdVector<stdString> &stat_strings,
                                stdVector<SeverityInfo> &sevr_infos)
{
    xmlrpc_value *result, *hows, *stats, *sevrs, *element;
    xmlrpc_int32 num;
    xmlrpc_bool has_value, txt_stat;
    const char   *str;
    size_t       count, len, i;
    result = xmlrpc_client_call(&env, (char *)URL, "archiver.info", "()");
    if (log_fault())
        return false;
    xmlrpc_parse_value(&env, result, "{s:i,s:s#,s:A,s:A,s:A,*}",
                       "ver",  &num,
                       "desc", &str, &len,
                       "how",  &hows,
                       "stat", &stats,
                       "sevr", &sevrs);
    if (log_fault())
        return false;
    version = num;
    description.assign(str, len);
    // 'how' array
    count = xmlrpc_array_size(&env, hows);
    how_strings.reserve(count);
    for (i=0; i<count; ++i)
    {
        element = xmlrpc_array_get_item(&env, hows, i);
        if (log_fault())
            return false;
        xmlrpc_parse_value(&env, element, "s", &str);
        if (log_fault())
            return false;
        how_strings.push_back(stdString(str));
    }
    // 'stat' array
    count = xmlrpc_array_size(&env, stats);
    stat_strings.reserve(count);
    for (i=0; i<count; ++i)
    {
        element = xmlrpc_array_get_item(&env, stats, i);
        if (log_fault())
            return false;
        xmlrpc_parse_value(&env, element, "s", &str);
        if (log_fault())
            return false;
        stat_strings.push_back(stdString(str));
    }
    // 'sevr' array
    count = xmlrpc_array_size(&env, sevrs);
    stat_strings.reserve(count);
    SeverityInfo info;
    for (i=0; i<count; ++i)
    {
        element = xmlrpc_array_get_item(&env, sevrs, i);
        if (log_fault())
            return false;
        xmlrpc_parse_value(&env, element, "{s:i,s:s#,s:b,s:b,*}",
                           "num",  &num,
                           "sevr", &str, &len,
                           "has_value",  &has_value,
                           "txt_stat", &txt_stat);
        if (log_fault())
            return false;
        info.num = num;
        info.text.assign(str, len);
        info.has_value = has_value;
        info.txt_stat = txt_stat;
        sevr_infos.push_back(info);
    }
    xmlrpc_DECREF(result);
    return true;
}

bool ArchiveDataClient::getArchives(stdVector<ArchiveInfo> &archives)
{
    xmlrpc_value *result, *element;
    xmlrpc_int32 key;
    const char   *name, *path;
    size_t       count, i, n_len, p_len;
    ArchiveInfo  info;
    result = xmlrpc_client_call(&env, (char *)URL, "archiver.archives", "()");
    if (log_fault())
        return false;
    count = xmlrpc_array_size(&env, result);
    archives.reserve(count);
    for (i=0; i<count; ++i)
    {
        element = xmlrpc_array_get_item(&env, result, i);
        if (log_fault())
            return false;
        xmlrpc_parse_value(&env, element, "{s:i,s:s#,s:s#,*}",
                           "key",  &key,
                           "name", &name, &n_len,
                           "path", &path, &p_len);
        if (log_fault())
            return false;
        info.key = key;
        info.name.assign(name, n_len);
        info.path.assign(path, p_len);
        archives.push_back(info);
    }
    xmlrpc_DECREF(result);
    return true;
}

bool ArchiveDataClient::getNames(int key, const stdString &pattern, stdVector<NameInfo> &names)
{
    xmlrpc_value *result, *element;
    const char   *name;
    xmlrpc_int32 start_sec, start_nano, end_sec, end_nano;
    size_t       count, i, len;
    NameInfo     info;
    result = xmlrpc_client_call(&env, (char *)URL, "archiver.names", "(is)",
                                (xmlrpc_int32) key, pattern.c_str());
    if (log_fault())
        return false;
    count = xmlrpc_array_size(&env, result);
    names.reserve(count);
    for (i=0; i<count; ++i)
    {
        element = xmlrpc_array_get_item(&env, result, i);
        if (log_fault())
            return false;
        xmlrpc_parse_value(&env, element, "{s:s#,s:i,s:i,s:i,s:i,*}",
                           "name",       &name, &len,
                           "start_sec",  &start_sec,
                           "start_nano", &start_nano,
                           "end_sec",    &end_sec,
                           "end_nano",   &end_nano);
        if (log_fault())
            return false;
        info.name.assign(name, len);
        pieces2epicsTime(start_sec, start_nano, info.start);
        pieces2epicsTime(end_sec, end_nano, info.end);
        names.push_back(info);
    }
    xmlrpc_DECREF(result);
    return true;
}

bool ArchiveDataClient::getValues(int key, stdVector<stdString> &names,
                                  const epicsTime &start, const epicsTime &end,
                                  int count, int how,
                                  value_callback callback, void *callback_arg, double interval)
{
    xmlrpc_value *name_array, *result, *element;
    size_t n;
    name_array = xmlrpc_build_value(&env, "()");
    if (log_fault())
        return false;
     for (n=0; n<names.size(); ++n)
    {
        element = xmlrpc_build_value(&env, "s", names[n].c_str());
        if (log_fault())
            return false;
        xmlrpc_array_append_item(&env, name_array, element);
        if (log_fault())
            return false;
        xmlrpc_DECREF(element);
    }
    xmlrpc_int32 start_sec, start_nano, end_sec, end_nano;
    epicsTime2pieces(start, start_sec, start_nano);
    epicsTime2pieces(end, end_sec, end_nano);

	end_nano = (xmlrpc_int32)interval;

#if 0
    result = xmlrpc_client_call(&env, (char *)URL,
                                "archiver.values", "(iViiiiiid)",
                                (xmlrpc_int32)key, name_array,
                                start_sec, start_nano, end_sec, end_nano,
                                (xmlrpc_int32)count, (xmlrpc_int32)how, (xmlrpc_double)interval);
#endif
    result = xmlrpc_client_call(&env, (char *)URL,
                                "archiver.values", "(iViiiiii)",
                                (xmlrpc_int32)key, name_array,
                                start_sec, start_nano, end_sec, end_nano,
                                (xmlrpc_int32)count, (xmlrpc_int32)how);

    if (log_fault())
        return false;
    xmlrpc_DECREF(name_array);
    size_t channel_count = xmlrpc_array_size(&env, result);
    xmlrpc_value *channel_result;
    for (n=0; n<channel_count; ++n)
    {
        channel_result = xmlrpc_array_get_item(&env, result, n);
        if (log_fault())
            return false;
        if (!decode_channel(channel_result, n, callback, callback_arg))
            return false;
    }
    xmlrpc_DECREF(result);
    return true;
}

bool ArchiveDataClient::log_fault()
{
    if (!env.fault_occurred)
        return false;
    LOG_MSG("ArchiveDataClient XML-RPC Fault: %s (%d)\n",
            env.fault_string, env.fault_code);
    return true;
}

bool ArchiveDataClient::decode_channel(xmlrpc_value *channel,
                                       size_t n,
                                       value_callback callback,
                                       void *callback_arg)
{
    char *name;
    xmlrpc_value *meta, *data;
    xmlrpc_int32 type, count;
    xmlrpc_parse_value(&env, channel, "{s:s,s:V,s:i,s:i,s:A,*}",
                       "name", &name,
                       "meta", &meta,
                       "type", &type,
                       "count", &count,
                       "values", &data);
    if (log_fault())
        return false;
    CtrlInfo ctrlinfo;
    if (!decode_meta(meta, ctrlinfo))
        return false;
    return decode_data(name, type, count, data, ctrlinfo,
                       n, callback, callback_arg);
}

// Dump 'meta' part of returned value
bool ArchiveDataClient::decode_meta(xmlrpc_value *meta, CtrlInfo &ctrlinfo)
{
    xmlrpc_int32 type;
    xmlrpc_parse_value(&env, meta, "{s:i,*}", "type", &type);
    if (log_fault())
        return false;
    if (type == 0)
    {
        xmlrpc_value *element, *states;
        xmlrpc_parse_value(&env, meta, "{s:A,*}", "states", &states);
        if (log_fault())
            return false;
        size_t i, total, len, count = xmlrpc_array_size(&env, states);
        const char *str;
        total = 0;
        for (i=0; i<count; ++i)
        {
            element = xmlrpc_array_get_item(&env, states, i);
            if (log_fault())
                return false;
            xmlrpc_parse_value(&env, element, "s#", &str, &len);
            if (log_fault())
                return false;
            total += len+1;
        }
        ctrlinfo.allocEnumerated(count, total);
        for (i=0; i<count; ++i)
        {
            element = xmlrpc_array_get_item(&env, states, i);
            if (log_fault())
                return false;
            xmlrpc_parse_value(&env, element, "s", &str);
            if (log_fault())
                return false;
            ctrlinfo.setEnumeratedString(i, str);
        }
    }
    else if (type == 1)
    {
        double disp_high, disp_low, alarm_high, alarm_low, warn_high, warn_low;
        xmlrpc_int32 prec;
        char *units;   
        xmlrpc_parse_value(&env, meta, "{s:d,s:d,s:d,s:d,s:d,s:d,s:i,s:s,*}",
                           "disp_high", &disp_high,
                           "disp_low", &disp_low,
                           "alarm_high", &alarm_high,
                           "alarm_low", &alarm_low,
                           "warn_high", &warn_high,
                           "warn_low", &warn_low,
                           "prec", &prec,
                           "units", &units);
        if (log_fault())
            return false;
        ctrlinfo.setNumeric(prec, units, disp_low, disp_high,
                            alarm_low, warn_low, warn_high, alarm_high);
    }
    else
    {
        LOG_MSG("unknown meta type %d\n", type);
        return false;
    }
    return true;
}

bool ArchiveDataClient::decode_data(const char *name,
                                    xmlrpc_int32 type, xmlrpc_int32 count,
                                    xmlrpc_value *data_array,
                                    CtrlInfo &ctrlinfo,
                                    size_t n,
                                    value_callback callback,
                                    void *callback_arg)
{
    size_t       i, num, v, v_num;
    xmlrpc_int32 stat, sevr, secs, nano;
    xmlrpc_value *data, *value_array, *value;
    epicsTime    stamp;
    DbrType      dbr_type;
    RawValue::Data *raw_value;    
    switch (type)
    {
        case XML_STRING: dbr_type = DBR_TIME_STRING;  break;
        case XML_ENUM:   dbr_type = DBR_TIME_ENUM;    break;
        case XML_INT:    dbr_type = DBR_TIME_LONG;    break;
        case XML_DOUBLE: dbr_type = DBR_TIME_DOUBLE;  break;
        default:
            LOG_MSG("Cannot decode data type %d\n", type);
            return false;
    }
    raw_value = RawValue::allocate(dbr_type, count, 1);
    num = xmlrpc_array_size(&env, data_array);

	//leesi maximum array = 10000; why?
	printf("Num: %d\n", num );

    if (log_fault())
        return false;
    for (i=0; i<num; ++i)
    {
        data = xmlrpc_array_get_item(&env, data_array, i);
        xmlrpc_parse_value(&env, data, "{s:i,s:i,s:i,s:i,s:A,*}",
                           "stat", &stat, "sevr", &sevr,
                           "secs", &secs, "nano", &nano,
                           "value", &value_array);
        if (log_fault())
            return false;
        pieces2epicsTime(secs, nano, stamp);
        RawValue::setStatus(raw_value, stat, sevr);
        RawValue::setTime(raw_value, stamp);
        v_num = xmlrpc_array_size(&env, value_array);
        if (log_fault())
            return false;
        if (v_num != (size_t)count)
        {
            LOG_MSG("value size discrepancy, %zu != %zu\n", v_num, (size_t)count);
            return false;
        }
        for (v=0; v<v_num; ++v)
        {
            value = xmlrpc_array_get_item(&env, value_array, v);
            if (log_fault())
                return false;
            switch (dbr_type)
            {
                case DBR_TIME_STRING:
                {
                    if (v > 0)
                        break;
                    const char *txt;
                    size_t len;
                    xmlrpc_parse_value(&env, value, "s#", &txt, &len);
                    if (len >= MAX_STRING_SIZE)
                        len = MAX_STRING_SIZE-1;
                    memcpy(((dbr_time_string *)raw_value)->value, txt, len);
                    ((dbr_time_string *)raw_value)->value[len] = '\0';
                }
                break;
                case DBR_TIME_ENUM:
                {
                    dbr_enum_t *val = &((dbr_time_enum *)raw_value)->value;
                    xmlrpc_int32 ival;
                    xmlrpc_parse_value(&env, value, "i", &ival);
                    val[v] = ival;
                    if (log_fault())
                        return false;
                }
                break;
                case DBR_TIME_LONG:
                {
                    dbr_long_t *val = &((dbr_time_long *)raw_value)->value;
                    xmlrpc_int32 ival;
                    xmlrpc_parse_value(&env, value, "i", &ival);
                    val[v] = ival;
                    if (log_fault())
                        return false;
                }
                break;
                case DBR_TIME_DOUBLE:
                {
                    double *val = &((dbr_time_double *)raw_value)->value;
                    xmlrpc_parse_value(&env, value, "d", &val[v]);
                    if (log_fault())
                        return false;
                }
                break;
            }
        }
        if (!callback(callback_arg, name, n, i,
                      ctrlinfo, dbr_type, count, raw_value))
            break;
    }
    RawValue::free(raw_value);
    return true;
}

