POLLING_URL_1 = 'https://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=polling&t=OLd_JZ8'
POLLING_URL_2 = 'https://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=polling&t=OLd_JZ8-&sid={}'
POLLING_URL_3 = 'https://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=polling&t=OLd_JZY_&sid={}'
POLLING_URL_4 = 'https://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=polling&t=OLd_JZ8&sid={}'
POLLING_URL_5 = 'https://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=polling&t=OLd_JZ8&sid={}'

HEADER = {
        'Accept': '*/*',
        'Accept-Encoding': 'gzip, deflate, br',
        'Accept-Language': 'en-US,en;q=0.9',
        'Connection': 'keep-alive',
        'Sec-Fetch-Dest': 'empty',
        'Sec-Fetch-Mode': 'cors',
        'Sec-Fetch-Site': 'same-origin',
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.83 Safari/537.36'
        }
HEADER_WSS = {
'Host': 'bigapple.cytu.be:8443',
'Connection': 'Upgrade',
'Pragma': 'no-cache',
'Cache-Control': 'no-cache',
'Upgrade': 'websocket',
'Origin': 'https://cytu.be',
'Sec-WebSocket-Version': '13',
'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 Safari/537.36',
'Accept-Encoding': 'gzip, deflate, br',
'Accept-Language': 'en-US,en;q=0.9',
'Sec-GPC': '1',
'Sec-WebSocket-Key': '7wfF6vVBt2fOUj5JRhVoPA==',
'Sec-WebSocket-Extensions': 'permessage-deflate; client_max_window_bits'}

WSS_URL = 'wss://bigapple.cytu.be:8443/socket.io/?EIO=4&transport=websocket&sid={}'

class Parser:
    '''
        Makes socket messages into a python objects if possible and returns a representation (dict, df, ts, ...) thereof.
    '''
    def parser(self, message):
        #def pyify(obj):
        #    # Check if obj is already a pyobj
        #    if not type(obj) in [str]: return obj
        #    try: return eval(obj)
        #    except Exception as e: print('Parser::parser: Error {}'.format(e))

        ## Check for bools
        #pretty = message.replace('true', 'True')
        #pretty = pretty.replace('false', 'False')
        #pretty = pretty.replace('null', 'None')

        # Send the raw payload to a specific sub-parser
        try:
            res = getattr(self, message_type)(raw_payload)
            return message_type, res
        except Exception as e: print('Parser::parser: Error while sending a message of type "{}" to sub-parser:\n{}'.format(message_type), e)
