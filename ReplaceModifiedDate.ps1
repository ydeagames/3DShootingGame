$sel = '\${' + 'modified_date' + '}'

foreach ($file in Get-ChildItem . * -Recurse -Force | Select-String $sel -casesensitive)
{
    Write-Host "Processing " -ForegroundColor Red -NoNewline
    Write-Host $file.Path
    $gitModified = git log -1 --pretty="format:%ci" $file.Path
    Write-Host 'Modified: ' + $gitModified
    ((Get-Content -path $file.Path -Raw) -replace $sel,$gitModified) | Set-Content -Path $file.Path
}
Write-Host "All files has been recoded" -ForegroundColor Green