#ifndef DMA_DMA_H_
#define DMA_DMA_H_

#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		                    DMA typedefs     					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef void (* dma_callback_t)(void);

typedef struct {
  uint32_t SADDR;                               /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
  uint16_t SOFF;                                /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
  uint16_t ATTR;                                /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
  union {                                       
    uint32_t NBYTES_MLNO;                       /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
    uint32_t NBYTES_MLOFFNO;                    /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
    uint32_t NBYTES_MLOFFYES;                   /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
  };
  uint32_t SLAST;                               /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
  uint32_t DADDR;                               /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
  uint16_t DOFF;                                /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
  union {                                       
    uint16_t CITER_ELINKNO;                     /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
    uint16_t CITER_ELINKYES;                    /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
  };
  uint32_t DLAST_SGA;                           /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
  uint16_t CSR;                                 /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
  union {                                       
    uint16_t BITER_ELINKNO;                     /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
    uint16_t BITER_ELINKYES;                    /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
  };
}dma_TCD_t;


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//		             External function headers 					//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void dma_init();
void dma_push_TCD_to_channel(uint8_t channel, dma_TCD_t tcd);
void dma_assoc_callback_to_channel(uint8_t channel, dma_callback_t callback);
bool dma_get_finished_transfer(int channel);
void dma_enable_major_loop_irq(uint8_t channel_number, bool ie);
void dma0_enable_erq();

#endif /* DMA_DMA_H_ */
