#include "escorepch.h"
#pragma hdrstop

#include "EsCompositeFile.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

//#define STR_BLOCK_IDX _("Block index")
//
//// just create empty composite file object
//EsCompositeFile::EsCompositeFile() :
//m_newFile(true),
//m_locTableDirty(false)
//{
//}
//
//// return current file path
//EsString EsCompositeFile::getPath() const
//{
//	return m_file.pathGet();
//}
//
//// create new composite file object, resettings all existing contents
//void EsCompositeFile::create(const EsString& file, const EsString& baseFile)
//{
//	// check that baseFile exists and it's not composite
//	const EsPath& f = EsPath::FileName(baseFile);
//	wxString fn = f.GetFullPath();
//	if( f.FileExists() && f.IsFileReadable() )
//	{
//		wxFileInputStream fs(fn);
//		if( fs.IsOk() )
//		{
//			LoctableHeader hdr;
//			// try read locHeader
//			if( wxInvalidOffset != fs.SeekI( -static_cast<wxFileOffset>(sizeof(LoctableHeader)), wxFromEnd ) )
//			{
//				fs.Read(&hdr, sizeof(LoctableHeader));
//				if( fs.LastRead() == sizeof(LoctableHeader) )
//				{
//					// check if it's really locheader and issue an exception
//					if( hdr.isGuidOk() )
//						EsException::Throw(
//							_("File '%s' is composite. Composite file may not be used as base block for composite file."), fn.t_str());
//				}
//			}
//			// read entire file into the base block
//			EsBinBuffer block( fs.GetLength() );
//			fs.SeekI(0);
//			fs.Read( &block[0], block.size() );
//			if( fs.LastRead() == block.size() )
//				// call main create implementation
//				create(file, block);
//			else
//				EsException::Throw(
//					_("Error reading base file '%s'."), fn.t_str());
//		}
//		else
//			EsException::Throw(
//				_("Could not open file '%s' for reading."), fn.t_str() );
//	}
//	else
//		EsException::Throw(
//			_("File '%s' does not exist or is not accessible for reading."), fn.t_str() );
//}
//
//void EsCompositeFile::create(const EsString& file, const EsBinBuffer& baseBlock)
//{
//	if( !baseBlock.empty() )
//	{
//		// reset existing location table
//		resetLoctable();
//		// create new composite file
//		m_file = EsPath::FileName( file );
//		// check file existence
//		if( m_file.FileExists() )
//			EsException::Throw(_("File '%s' already exists."), file.c_str());
//		// create target file
//		wxFileOutputStream fs(m_file.GetFullPath());
//		if( fs.IsOk() )
//		{
//			// write base block to it
//			fs.Write( &baseBlock[0], baseBlock.size() );
//			// update loctable in file
//			updateLoctable(fs);
//		}
//		else
//			EsException::Throw(_("Cannot create file '%s'."), file.c_str());	
//	}
//	else
//		EsException::Throw(_("Could not create composite file from empty base block."));
//}
//
//// append composite block to the file
//void EsCompositeFile::appendBlock(const EsString& file, esU32 type, esU32 info)
//{
//	// check that baseFile exists and it's not composite
//	EsPath f = EsPath::FileName(file);
//	wxString fn = f.GetFullPath();
//	if( f.FileExists() && f.IsFileReadable() )
//	{
//		wxFileInputStream fs(fn);
//		if( fs.IsOk() )
//		{
//			// read entire file into the base block
//			EsBinBuffer block( fs.GetLength() );
//			fs.SeekI(0);
//			fs.Read( &block[0], block.size() );
//			if( fs.LastRead() == block.size() )
//				// call main appendBlock implementation
//				appendBlock(block, type, info);
//			else
//				EsException::Throw(
//					_("Error reading block file '%s'."), fn.t_str());
//		}
//		else
//			EsException::Throw(
//				_("Could not open file '%s' for reading."), fn.t_str() );
//	}
//	else
//		EsException::Throw(
//			_("File '%s' does not exist or is not accessible for reading."), fn.t_str() );
//}
//
//void EsCompositeFile::appendBlock(const EsBinBuffer& block, esU32 type, esU32 info)
//{
//	if( !block.empty() )
//	{
//		if( type < KnownBlockType )
//		{
//			LoctableEntry entry;
//			entry.m_type = type;
//			entry.m_info = info;
//			entry.m_len = block.size();
//			entry.m_crc32 = EsCRC32_IEEE802_3(block).get_value();
//			esU32 offsDelta = entry.m_len + sizeof(entry);
//			entry.m_offs = getLoctableLen()+offsDelta;
//			// update file
//			EsBinBuffer tmp;
//			getAllExceptLoctable(tmp);
//			wxFileOutputStream fso(m_file.GetFullPath());
//			if( fso.IsOk() )
//			{
//				// write previous contents
//				fso.Write(&tmp[0], tmp.size());
//				ES_ASSERT(fso.LastWrite() == tmp.size());
//				// write block
//				fso.Write(&block[0], block.size());
//				ES_ASSERT(fso.LastWrite() == block.size());
//				// sel loctable dirty flag
//				m_locTableDirty = true;
//				// update all existing loctable entries offsets for offsDelts
//				for( size_t idx = 0; idx < m_locEntries.size(); ++idx)
//					m_locEntries[idx].m_offs += offsDelta;
//				// append this entry
//				m_locEntries.push_back( entry );
//				// update loctable in file
//				updateLoctable(fso);
//			}
//			else
//				EsException::Throw(_("Could not append block to the composite file."));
//		}
//		else
//			EsException::Throw(_("Could not append block of unknown type to the composite file."));
//	}
//	else
//		EsException::Throw(_("Could not append empty block to the composite file."));
//}
//
//// read and parse existing composite file
//void EsCompositeFile::load(const EsString& file)
//{
//	// first, check file existence and accessibility for read
//	EsPath f = EsPath::FileName( file );
//	wxString fn = f.GetFullPath();
//	if( f.FileExists() && f.IsFileReadable() )
//	{
//		// reset current loctable data
//		resetLoctable();
//		// open and read file contents, starting from loctable
//		m_file = f;
//		wxFileInputStream fs(fn);
//		if( fs.IsOk() )
//		{
//			// read loctable
//			fs.SeekI( -static_cast<wxFileOffset>(sizeof(LoctableHeader)), wxFromEnd );
//			fs.Read( &m_locHdr, sizeof(LoctableHeader) );
//			if( fs.LastRead() == sizeof(LoctableHeader) && m_locHdr.isGuidOk() && (m_locHdr.m_size % sizeof(LoctableEntry)) == 0 )
//			{
//				EsBinBuffer entries( m_locHdr.m_size );
//				// read loctable entries
//				fs.SeekI( -static_cast<wxFileOffset>(getLoctableLen()), wxFromEnd );
//				fs.Read( &entries[0], entries.size() );
//				if( fs.LastRead() == entries.size() )
//				{
//					// calculate and check crc for loctable
//					EsCRC32_IEEE802_3 crc32(entries);
//					crc32.update( reinterpret_cast<const esU8*>(&m_locHdr), sizeof(LoctableHeader)-sizeof(esU32) );
//					if( crc32.get_value() == m_locHdr.m_crc32 )
//					{
//						// parse table entries
//						m_locEntries.reserve( m_locHdr.m_size / sizeof(LoctableEntry) );
//						LoctableEntry* pos = reinterpret_cast<LoctableEntry*>(&entries[0]);
//						LoctableEntry* end = reinterpret_cast<LoctableEntry*>((&entries[0]) + m_locHdr.m_size);
//						while( pos < end )
//							m_locEntries.push_back( *pos++ );
//						m_newFile = false;
//					}
//					else
//						EsException::Throw(
//							_("File '%s' table CRC is corrupt."), fn.t_str());
//				}
//				else
//					EsException::Throw(
//						_("Could not read the table. File '%s' is either not composite, or corrupt."), fn.t_str());
//			}
//			else
//				EsException::Throw(
//					_("Could not find the header. File '%s' is either not composite, or corrupt."), fn.t_str());
//		}
//		else
//			EsException::Throw(
//				_("Could not read the composite file '%s'."), fn.t_str());
//	}
//	else
//		EsException::Throw(
//			_("Could not access the composite file '%s'. File is either non-existent or not accessible for read."), fn.t_str());
//}
//
//// access composite blocks
//size_t EsCompositeFile::getBlocksCount() const
//{
//	return m_locEntries.size();
//}
//
//esU32 EsCompositeFile::getBlockType(size_t idx) const
//{
//	EsNumericCheck::checkRangeInteger(0, m_locEntries.size()-1, idx, STR_BLOCK_IDX);
//	return m_locEntries[idx].m_type;
//}
//
//esU32 EsCompositeFile::getBlockInfo(size_t idx) const
//{
//	EsNumericCheck::checkRangeInteger(0, m_locEntries.size()-1, idx, STR_BLOCK_IDX);
//	return m_locEntries[idx].m_info;
//}
//
//EsBinBuffer EsCompositeFile::getBlock(size_t idx) const
//{
//	EsNumericCheck::checkRangeInteger(0, m_locEntries.size()-1, idx, STR_BLOCK_IDX);
//	EsBinBuffer block( m_locEntries[idx].m_len );
//	
//	wxString file = m_file.GetFullPath();
//	wxFileInputStream fsi( file );
//	if( fsi.IsOk() )
//	{
//		fsi.SeekI( -static_cast<wxFileOffset>(m_locEntries[idx].m_offs), wxFromEnd );
//		fsi.Read( &block[0], block.size() );
//		if( fsi.LastRead() == block.size() )
//		{
//			// check block crc
//			EsCRC32_IEEE802_3 crc32(block);
//			if( crc32.get_value() != m_locEntries[idx].m_crc32 )
//				EsException::Throw(_("CRC error in block %d in file '%s'"),
//					idx, file.t_str() );
//		}
//		else
//			EsException::Throw(_("Could not read block %d from file '%s'"),
//				idx, file.t_str() );
//	}
//	else
//		EsException::Throw(_("Could not open file '%s' for reading"),
//			file.t_str() );
//	
//	return block;
//}
//	
//// internal services
////
//// get all file contents except loctable as binary buffer
//void EsCompositeFile::getAllExceptLoctable(EsBinBuffer& buff)
//{
//	wxFileInputStream fsi(m_file.GetFullPath());
//	ES_ASSERT(fsi.IsOk());
//	buff.resize( fsi.GetLength() - getLoctableLen() );
//	fsi.Read(&buff[0], buff.size());
//	ES_ASSERT(fsi.LastRead() == buff.size());
//}
//
//// reset loctable
//void EsCompositeFile::resetLoctable()
//{
//	m_locHdr = LoctableHeader();
//	m_locEntries.clear();
//	m_locTableDirty = true;
//}
//
//// recalculate and update loctable in file
//void EsCompositeFile::updateLoctable(wxFileOutputStream& fs)
//{
//	if( m_locTableDirty )
//	{
//		// update total loctable crc
//		updateLoctableHdr();
//		// write loctable to the target file
//		writeLoctable(fs);
//		// clear dirty flag
//		m_locTableDirty = false;
//	}
//}
//
//// update total loctable crc and size fields
//void EsCompositeFile::updateLoctableHdr()
//{
//	EsCRC32_IEEE802_3 crc32;
//	// calculate loctable header crc for all existing entries
//	m_locHdr.m_size = m_locEntries.size()*sizeof(LoctableEntry);
//	for(size_t idx = 0; idx < m_locEntries.size(); ++idx)
//		crc32.update( reinterpret_cast<const esU8*>(&m_locEntries[idx]), sizeof(LoctableEntry) );
//	crc32.update( reinterpret_cast<const esU8*>(&m_locHdr), sizeof(LoctableHeader)-sizeof(esU32) );
//	m_locHdr.m_crc32 = crc32.get_value();	
//}
//
//// write loctable to the target file
//void EsCompositeFile::writeLoctable(wxFileOutputStream& fs)
//{
//	// NB! as soon as this call is always part of internal file update,
//	// we assume that write always starts at the current file position
//	ES_ASSERT( fs.IsOk() );
//	// first, write loctable entries
//	for(size_t idx = 0; idx < m_locEntries.size(); ++idx)
//		fs.Write( &m_locEntries[idx], sizeof(LoctableEntry) );
//	// last - write loctable header
//	fs.Write( &m_locHdr, sizeof(LoctableHeader) );
//}
//
//// get offset from the end of the file to the beginning of loctable
//esU32 EsCompositeFile::getLoctableLen() const
//{
//	return sizeof(LoctableHeader) + m_locHdr.m_size;
//}

