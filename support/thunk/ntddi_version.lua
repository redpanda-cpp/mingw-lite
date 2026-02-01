function ntddi_version()
  local thunk_level = get_config('thunk-level')
  if thunk_level == '10.0.19041' then
    return ntddi_win10_vb()
  elseif thunk_level == '10.0.10240' then
    return ntddi_win10()
  elseif thunk_level == '6.3' then
    return ntddi_winblue()
  elseif thunk_level == '6.2' then
    return ntddi_win8()
  elseif thunk_level == '6.1' then
    return ntddi_win7()
  elseif thunk_level == '6.0' then
    return ntddi_vista()
  elseif thunk_level == '5.2' then
    return ntddi_ws03()
  elseif thunk_level == '5.1' then
    return ntddi_winxp()
  elseif thunk_level == '5.0' then
    return ntddi_win2k()
  elseif thunk_level == '4.0' then
    return ntddi_win4()
  elseif thunk_level == '3.9999+4.90' then
    return ntddi_winme()
  elseif thunk_level == '3.9999+4.10' then
    return ntddi_win98()
  elseif thunk_level == '3.9999+4.00' then
    return ntddi_win95()
  else
    return 0
  end
end

function ntddi_win10_vb()
  return 0x0A000008
end

function ntddi_win10()
  return 0x0A000000
end

function ntddi_winblue()
  return 0x06030000
end

function ntddi_win8()
  return 0x06020000
end

function ntddi_win7()
  return 0x06010000
end

function ntddi_vista()
  return 0x06000000
end

function ntddi_ws03()
  return 0x05020000
end

function ntddi_winxp()
  return 0x05010000
end

function ntddi_win2k()
  return 0x05000000
end

function ntddi_win4()
  return 0x04000000
end

function ntddi_winme()
  return 0x03FF0000 + 3000
end

function ntddi_win98()
  return 0x03FF0000 + 1998
end

function ntddi_win95()
  return 0x03FF0000 + 950
end
